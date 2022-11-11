//
//  ffdecoder.hpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/11.
//

#ifndef ffdecoder_hpp
#define ffdecoder_hpp

#include <stdio.h>
#include "ffmpegInclude.h"

typedef void (*DecodeCallback)(int status, int type, AVFrame *frame);

class FFdecoder {
    AVFormatContext *_deFormatCtx = NULL;
    AVCodecContext *_decodeCtx = NULL;
    AVDictionary *_deOptions = NULL;
    int _videoIndex = -1;

    AVFrame *_frame = NULL;
    AVPacket *_packet = NULL;
    
    DecodeCallback _callback = NULL;
    const char* _file = NULL;
    
public:
    FFdecoder(const char* file, DecodeCallback callback);
    ~FFdecoder();
    void start();
    int cancel();
    
private:
    void release();
    AVStream* findBestVideoStream(AVFormatContext *deFoCtx);
    void errorCallback(const char * __restrict message, ...);
};

#endif /* ffdecoder_hpp */
