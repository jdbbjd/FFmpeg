//
//  VTDecoder.hpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/15.
//

#ifndef VTDecoder_hpp
#define VTDecoder_hpp

#include <stdio.h>
#include <VideoToolbox/VideoToolbox.h>

class VTDecoder {
    
    const char *_file_path = NULL;
    VTDecompressionSessionRef *_decomsressionSession;
    
public:
    VTDecoder(const char* file_path);
    ~VTDecoder();
    
    int start();
    int cancel();
    
private:
    void release();
};

#endif /* VTDecoder_hpp */
