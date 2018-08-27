// Some of the code here was taken from this file: https://github.com/SRombauts/SimplexNoise/blob/master/src/SimplexNoise.cpp
// Here's a license notice:

// The MIT License (MIT)
// 
// Copyright (c) 2012-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <L/src/constants.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/Rand.h>
#include <L/src/rendering/Texture.h>

using namespace L;

// Hash lookup table as defined by Ken Perlin. This is a random permutation of numbers 0-255
static uint8_t p[] {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
};

static inline uint8_t hash(int32_t i) {
  return p[static_cast<uint8_t>(i)];
}

static float grad(int32_t hash, float x, float y) {
  const int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
  const float u = h < 4 ? x : y;  // into 8 simple gradient directions,
  const float v = h < 4 ? y : x;
  return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
}

static inline int32_t fastfloor(float fp) {
  int32_t i = static_cast<int32_t>(fp);
  return (fp < i) ? (i - 1) : (i);
}

static float perlin(float x, float y) {
  float n0, n1, n2;   // Noise contributions from the three corners

                      // Skewing/Unskewing factors for 2D
  static const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
  static const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

                                         // Skew the input space to determine which simplex cell we're in
  const float s = (x + y) * F2;  // Hairy factor for 2D
  const float xs = x + s;
  const float ys = y + s;
  const int32_t i = fastfloor(xs);
  const int32_t j = fastfloor(ys);

  // Unskew the cell origin back to (x,y) space
  const float t = static_cast<float>(i + j) * G2;
  const float X0 = i - t;
  const float Y0 = j - t;
  const float x0 = x - X0;  // The x,y distances from the cell origin
  const float y0 = y - Y0;

  // For the 2D case, the simplex shape is an equilateral triangle.
  // Determine which simplex we are in.
  int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
  if(x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    i1 = 1;
    j1 = 0;
  } else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
    i1 = 0;
    j1 = 1;
  }

  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
  // c = (3-sqrt(3))/6

  const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
  const float y1 = y0 - j1 + G2;
  const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
  const float y2 = y0 - 1.0f + 2.0f * G2;

  // Work out the hashed gradient indices of the three simplex corners
  const int gi0 = hash(i + hash(j));
  const int gi1 = hash(i + i1 + hash(j + j1));
  const int gi2 = hash(i + 1 + hash(j + 1));

  // Calculate the contribution from the first corner
  float t0 = 0.5f - x0*x0 - y0*y0;
  if(t0 < 0.0f) {
    n0 = 0.0f;
  } else {
    t0 *= t0;
    n0 = t0 * t0 * grad(gi0, x0, y0);
  }

  // Calculate the contribution from the second corner
  float t1 = 0.5f - x1*x1 - y1*y1;
  if(t1 < 0.0f) {
    n1 = 0.0f;
  } else {
    t1 *= t1;
    n1 = t1 * t1 * grad(gi1, x1, y1);
  }

  // Calculate the contribution from the third corner
  float t2 = 0.5f - x2*x2 - y2*y2;
  if(t2 < 0.0f) {
    n2 = 0.0f;
  } else {
    t2 *= t2;
    n2 = t2 * t2 * grad(gi2, x2, y2);
  }

  // Add contributions from each corner to get the final noise value.
  // The result is scaled to return values in the interval [-1,1].
  return 45.23065f * (n0 + n1 + n2);
}

static float octave_perlin(float x, float y, uint32_t octaves, float persistence, float lacunarity) {
  float total(0.f), frequency(1.f), amplitude(1.f), max_amplitude(0.f);
  for(uint32_t i(0); i<octaves; i++) {
    total += (perlin(x * frequency, y * frequency)*.5f+.5f) * amplitude;

    max_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }

  return total/max_amplitude;
}

static bool perlin_noise_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  uint32_t width(512), height(512), octaves(1);
  float persistence(.5f), lacunarity(2.f);

  slot.parameter("width", width);
  slot.parameter("height", height);
  slot.parameter("octaves", octaves);
  slot.parameter("persistence", persistence);
  slot.parameter("lacunarity", lacunarity);

  float* matrix(Memory::alloc_type<float>(width*height));
  const float x_mult(1.f/width), y_mult(1.f/height);

  for(uint32_t x(0); x<width; x++) {
    for(uint32_t y(0); y<height; y++) {
      matrix[x*width+y] = octave_perlin(x*x_mult, y*y_mult, octaves, persistence, lacunarity);
    }
  }

  slot.persistent = true;
  intermediate.width = width;
  intermediate.height = height;
  intermediate.format = VK_FORMAT_R8_UNORM;
  intermediate.binary = Buffer(width*height);

  for(uint32_t x(0); x<width; x++) {
    for(uint32_t y(0); y<height; y++) {
      ((uint8_t*)intermediate.binary.data())[x*width+y] = uint8_t(matrix[x*width+y]*255.f);
    }
  }

  Memory::free_type(matrix, width*height);

  return true;
}

void perlin_noise_module_init() {
  ResourceLoading<Texture>::add_loader("perlin_noise", perlin_noise_loader);
}
