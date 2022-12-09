//
//  ffencoder.hpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/11.
//

#ifndef ffencoder_hpp
#define ffencoder_hpp

#include <stdio.h>
#include "ffmpegInclude.h"

class FFEncoder {
    const char *_outPath = NULL;
    AVFormatContext *_outFormatCtx = NULL;
    AVStream *_outVideoStream = NULL;
    AVCodecContext *_encodeCtx = NULL;
    
    AVPacket *_packet = NULL;
    
    int _width = 0;
    int _height = 0;
    int _fps = 30;
    int _bit_rate = 200000;
    
    
public:
    FFEncoder(const char* outPath, int width, int height, int fps, int bit_rate);
    ~FFEncoder();
    int start();
    int encodeVideoFrame(AVFrame* frame);
    int cancel();
    int finish();
    
private:
    void release();
};

#endif /* ffencoder_hpp */
