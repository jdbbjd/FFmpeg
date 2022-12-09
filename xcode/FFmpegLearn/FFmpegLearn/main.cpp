//
//  main.cpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/9.
//

#include "ffmpegInclude.h"
#include "ffdecoder.hpp"
#include "ffencoder.hpp"
#include <string>
#include "ffrender.hpp"

const char* testVideo = "/xcode/Resources/in/IMG_1180.mov";
const char* testOutVideo = "/xcode/Resources/out/IMG_1180.mp4";
const char* testApng = "/xcode/Resources/in/in.png";

FFdecoder *_decoder = NULL;
static FFEncoder *_encoder = NULL;
static FFRenderer *_renderer = NULL;

void decodeCallBack(int status, int type, AVFrame *frame)
{
    if (frame) {
        _renderer->render(frame);
        _encoder->encodeVideoFrame(frame);
    }
    
    if (status == 1) {
        _encoder->finish();
    }else if(status == -1) {
        _encoder->cancel();
    }
}

void decodeFile(const char * filePath)
{
    AVFormatContext *deFormatCtx = NULL;
    AVCodecContext *deCodecCtx = NULL;
    const AVCodec *decodec = NULL;
    AVDictionary *inFormatOptions = NULL;
    
    int ret = avformat_open_input(&deFormatCtx, filePath, deFormatCtx->iformat, &inFormatOptions);
    
    ret = av_find_best_stream(deFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *stream = deFormatCtx->streams[ret];
    
    deCodecCtx = avcodec_alloc_context3(NULL);
    ret = avcodec_parameters_to_context(deCodecCtx, stream->codecpar);
    
    decodec = avcodec_find_decoder(deCodecCtx->codec_id);
    
    avcodec_open2(deCodecCtx, decodec, NULL);
    av_dump_format(deFormatCtx, 0, filePath, 0);
    
    avcodec_free_context(&deCodecCtx);
    avformat_close_input(&deFormatCtx);
    
}

auto lambda()
{
    std::string inPath = std::string("argv[1]");
    auto lambda = [&inPath](int status, int type, AVFrame *frame) {
        printf("in path %s\n", inPath.c_str());
        if (frame) {
            _renderer->render(frame);
            _encoder->encodeVideoFrame(frame);
        }
        
        if (status == 1) {
            _encoder->finish();
        }else if(status == -1) {
            _encoder->cancel();
        }
    };
    return lambda;
}

int main(int argc, const char * argv[]) {
    
    _renderer = new FFRenderer();
    
    std::string outPath = std::string(argv[1]) + testOutVideo;
    _encoder = new FFEncoder(outPath.c_str(), 1080, 1920, 30, 10000000);
    _encoder->start();
    
    std::string inPath = std::string(argv[1]) + testVideo;
    auto lb = lambda();
    _decoder = new FFdecoder(inPath.c_str(), lb);
    _decoder->start();
    
    return 0;
}

