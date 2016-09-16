#pragma once

#include "../types.h"

namespace L{
  float* bayerMatrix(const uint32_t width,const uint32_t height,float* out);
}
