//
//  ffencoder.cpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/11.
//

#include "ffencoder.hpp"

#define encode_time_base_den 600

FFEncoder::FFEncoder(const char* outPath, int width, int height, int fps, int bit_rate) : _outPath(outPath), _width(width), _height(height), _fps(fps), _bit_rate(bit_rate) {}

FFEncoder::~FFEncoder() {
    release();
}

int FFEncoder::start()
{
    printf("encode start\n");
    
    // set out format context
    int res = avformat_alloc_output_context2(&_outFormatCtx, NULL, "mp4", _outPath);
    
    if (res < 0) {
        fprintf(stderr, "Could not create out format context\n");
        return -1;
    }
    
    const AVOutputFormat *outFmt = _outFormatCtx->oformat;
    const AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    
    _encodeCtx = avcodec_alloc_context3(encoder);
    _encodeCtx->codec_id = AV_CODEC_ID_H264;
    _encodeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    _encodeCtx->bit_rate = _bit_rate;
    _encodeCtx->width = _width;
    _encodeCtx->height = _height;
    _encodeCtx->time_base = (AVRational){1, encode_time_base_den};
    _encodeCtx->framerate = (AVRational){_fps, 1};
    _encodeCtx->gop_size = 12;
    _encodeCtx->max_b_frames = 0;
    _encodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    
    avcodec_open2(_encodeCtx, encoder, NULL);
    
//    if (outFmt->flags & AVFMT_GLOBALHEADER) {
//        _encodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//    }
    
    // add new stream
    _outVideoStream = avformat_new_stream(_outFormatCtx, NULL);
    _outVideoStream->id = 0;

    avcodec_parameters_from_context(_outVideoStream->codecpar, _encodeCtx);
    
    if (!(outFmt->flags & AVFMT_NOFILE)) {
        res = avio_open(&_outFormatCtx->pb, _outPath, AVIO_FLAG_WRITE);
        if (res < 0) {
            fprintf(stderr, "Could not open out path\n");
            return -1;
        }
    }
    
    _packet = av_packet_alloc();
    
    res = avformat_write_header(_outFormatCtx, NULL);
    
    return 0;
}


int FFEncoder::encodeVideoFrame(AVFrame *frame) {
    if (frame) {
        printf("encode frame size: {%d, %d}, pts: %lld\n", frame->width, frame->height, frame->pts);
        
        int res = avcodec_send_frame(_encodeCtx, frame);
        if (res < 0) {
            fprintf(stderr, "Could not send frame!");
            return -1;
        }
        
        while (res >= 0) {
            res = avcodec_receive_packet(_encodeCtx, _packet);
            if (res == AVERROR_EOF || res == AVERROR(EAGAIN)) {
                av_packet_unref(_packet);
                return 0;
            }else if (res < 0)
            {
                av_packet_unref(_packet);
                fprintf(stderr, "Receive packet failed!");
                return -1;
            }

            av_packet_rescale_ts(_packet, _encodeCtx->time_base, _outVideoStream->time_base);
            _packet->stream_index = 0;
            
//            _packet->pts = av_rescale_q_rnd(_packet->pts,
//                                            _encodeCtx->time_base, _outVideoStream->time_base,
//                                       (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
//                                       );
//            _packet->dts = av_rescale_q_rnd(_packet->dts,
//                                            _encodeCtx->time_base,
//                                            _outVideoStream->time_base,
//                                       (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
//                                       );
            // pos 是 pkt 在文件索引当中的位置，因为封装格式不一样，header 大小不一样，
            // 则picket的位置有可能不一样的，所以不能用输入的pos
            // 设置pos = -1，不采用之前pkt的位置，它内部重算
//            _packet->pos = -1;
//            _packet->duration = av_rescale_q_rnd(_packet->duration,
//                                                 _encodeCtx->time_base,
//                                                 _outVideoStream->time_base,
//                                            (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
//                                            );
            
            res = av_interleaved_write_frame(_outFormatCtx, _packet);
            
            if (res < 0) {
                fprintf(stderr, "Error video muxing!");
                av_packet_unref(_packet);
                return -1;
            }
        }
    }
    
    printf("EncodeVideoFrame error, frame is NULL\n");
    return -1;
}

int FFEncoder::finish()
{
    printf("encode finish\n");
    int ret = avcodec_send_frame(_encodeCtx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not flush!");
    }else
    {
        ret = av_write_trailer(_outFormatCtx);
        if (ret < 0) {
            fprintf(stderr, "Could not finish!");
        }
    }
    release();
    return ret;
}

int FFEncoder::cancel()
{
    printf("encode cancel\n");
    release();
    return 0;
}

void FFEncoder::release()
{
    if (_encodeCtx) {
        avcodec_close(_encodeCtx);
        avcodec_free_context(&_encodeCtx);
    }
    if (_outFormatCtx) {
        avformat_free_context(_outFormatCtx);
        _outFormatCtx = NULL;
    }
    
    if (_packet) {
        av_packet_free(&_packet);
    }
}
