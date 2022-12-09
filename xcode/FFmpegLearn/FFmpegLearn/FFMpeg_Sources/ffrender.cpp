//
//  ffrender.cpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/11.
//

#include "ffrender.hpp"

void FFRenderer::render(AVFrame *frame)
{
    memset(frame->data[0], 0, frame->linesize[0] * frame->height / 2);
    memset(frame->data[1], 0, frame->linesize[1] * frame->height / 4);
    memset(frame->data[2], 0, frame->linesize[2] * frame->height / 4);
}
