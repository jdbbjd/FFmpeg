//
//  main.cpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/9.
//

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

const char* testVideo = "/Users/tl/Documents/workSpace/github/FFmpeg/fftools/xcode/Resources/in/IMG_1180.mov";
const char* testApng = "/Users/tl/Documents/workSpace/github/FFmpeg/fftools/xcode/Resources/in/in.png";

void decodeFile(const char * filePath)
{
    AVFormatContext *deFormatCtx = NULL;
    AVCodecContext *deCodecCtx = NULL;
    const AVCodec *decodec = NULL;
    AVDictionary *inFormatOptions = NULL;
    
    deFormatCtx = avformat_alloc_context();
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


int main(int argc, const char * argv[]) {
    decodeFile(testApng);
    return 0;
}
