//
//  ffdecoder.cpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/11.
//

#include "ffdecoder.hpp"
#include "FileUtils.hpp"

//FFdecoder::FFdecoder(const char* file, DecodeCallback callback) : _file(file), _callback(callback)
//{
//}


FFdecoder::FFdecoder(const char* file, std::function<void(int status, int type, AVFrame *frame)> callback) : _file(file), _callback(callback) {
    
}

void FFdecoder::start()
{
    int res = FileUtils::fileExist(_file);
    if (res < 0) {
        printf("The file is not exist\n");
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    res = avformat_open_input(&_deFormatCtx, _file, NULL, &_deOptions);
    if (res < 0 || _deFormatCtx == NULL) {
        printf("Open file failed!\n");
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    AVStream *videoStream = findBestVideoStream(_deFormatCtx);
    if (videoStream == NULL) {
        printf("Cant find video stream\n");
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    const AVCodec *decoder = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (decoder == NULL) {
        printf("Cant find decoder for id: %d\n", videoStream->codecpar->codec_id);
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    _decodeCtx = avcodec_alloc_context3(decoder);
    if (_decodeCtx == NULL) {
        printf("Cant find decoder for id: %d\n", videoStream->codecpar->codec_id);
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    res = avcodec_parameters_to_context(_decodeCtx, videoStream->codecpar);
    if (res < 0) {
        printf("Failed to copy parameters to decoder context!\n");
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    res = avcodec_open2(_decodeCtx, decoder, NULL);
    if (res < 0) {
        printf("Failed to open decode context!\n");
        if (_callback) {
            _callback(-1, -1, NULL);
        }
        return;
    }
    
    
    _frame = av_frame_alloc();
    _packet = av_packet_alloc();
    
    bool failed = false;
    while (av_read_frame(_deFormatCtx, _packet) >= 0)
    {
        printf("stream index: %d\n",_packet->stream_index);
        if (_packet->stream_index == _videoIndex) {
            res = avcodec_send_packet(_decodeCtx, _packet);
            if (res < 0) {
                printf("Failed to avcodec_send_packet\n");
                if (_callback) {
                    _callback(-1, 0, NULL);
                    failed = true;
                }
                break;
            }
            
            while (res >= 0) {
                res = avcodec_receive_frame(_decodeCtx, _frame);
                if (res < 0) {
                    if (res == AVERROR_EOF || res == AVERROR(EAGAIN))
                        continue;
                    printf("Failed to avcodec_receive_frame\n");
                    if (_callback) {
                        _callback(-1, 0, NULL);
                        failed = true;
                    }
                    break;
                }
                _callback(0, 0, _frame);
                av_frame_unref(_frame);
            }
            
            if (failed) {
                break;
            }
        }
        av_packet_unref(_packet);
    }
    if (!failed) {
        avcodec_send_packet(_decodeCtx, NULL);
        _callback(1, 0, NULL);
    }
}

AVStream * FFdecoder::findBestVideoStream(AVFormatContext *deFoCtx)
{
    int res = av_find_best_stream(deFoCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    _videoIndex = res;
    AVStream *stream = NULL;
    if (res >= 0 && res < deFoCtx->nb_streams) {
        stream = deFoCtx->streams[res];
    }
    return stream;
}

void FFdecoder::errorCallback(const char *message, ...)
{
}

void FFdecoder::release()
{
    if (_decodeCtx) {
        avcodec_free_context(&_decodeCtx);
    }
    if (_deFormatCtx) {
        avformat_close_input(&_deFormatCtx);
    }
    if (_frame) {
        av_frame_free(&_frame);
    }
    if (_packet) {
        av_packet_free(&_packet);
    }
    if (_deOptions) {
        av_dict_free(&_deOptions);
    }
}

FFdecoder::~FFdecoder()
{
    release();
}
