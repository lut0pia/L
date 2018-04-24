#pragma once

#include <cstdint> 

namespace L {
  float* bayerMatrix(const uint32_t width, const uint32_t height, float* out);
  float* void_and_cluster(uint32_t width, uint32_t height, float* out);
}
