//
//  ffrender.hpp
//  FFmpegLearn
//
//  Created by T-MBP on 2022/11/11.
//

#ifndef ffrender_hpp
#define ffrender_hpp
#include "ffmpegInclude.h"

#include <stdio.h>

class FFRenderer {
    
    
public:
    void render(AVFrame *frame);
};

#endif /* ffrender_hpp */
