#include "dither.h"

#include "../dev/debug.h"

using namespace L;

float* L::bayerMatrix(const uint32_t width,const uint32_t height,float* out){
  L_ASSERT_MSG(width==height,"Only square bayer matrices are supported.");
  const float size(width*height);
  for(uint32_t i(0); i<size; i++){
    uint32_t x(0),y(0),level(1),offset(width/2);
    while(offset>0){
      const uint32_t q((i/level)%4);
      switch(q){
        case 1: x += offset;
        case 2: y += offset; break;
        case 3: x += offset; break;
        default: break;
      }
      offset /= 2;
      level *= 4;
    }
    out[width*y+x] = float(1+i)/float(width*height+1);
  }
  return out;
}
