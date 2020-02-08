#include <L/src/constants.h>
#include <L/src/engine/Resource.inl>
#include <L/src/math/Rand.h>
#include <L/src/rendering/Texture.h>

L_PUSH_NO_WARNINGS

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

L_POP_NO_WARNINGS

using namespace L;

static const Symbol stb_perlin_symbol("stb_perlin"), ridge_symbol("ridge"), turbulence_symbol("turbulence");

static float turbulence(float x, float y, float lacunarity, float gain, int octaves, int wrap) {
  int i;
  float frequency = 1.0f;
  float amplitude = 1.0f;
  float sum = 0.0f;

  for(i = 0; i < octaves; i++) {
    float r = stb_perlin_noise3_internal(x*frequency, y*frequency, 0.f, wrap, wrap, wrap, (unsigned char)i)*amplitude;
    sum += (float)fabs(r);
    frequency *= lacunarity;
    amplitude *= gain;
  }
  return sum;
}
static float ridge(float x, float y, float lacunarity, float gain, int octaves, int wrap) {
  int i;
  float frequency = 1.0f;
  float prev = 1.0f;
  float amplitude = 0.5f;
  float sum = 0.0f;

  for(i = 0; i < octaves; i++) {
    float r = stb_perlin_noise3_internal(x*frequency, y*frequency, 0.f, wrap, wrap, wrap, (unsigned char)i);
    r = 1.f - (float)fabs(r);
    r = r*r;
    sum += r*amplitude*prev;
    prev = r;
    frequency *= lacunarity;
    amplitude *= gain;
  }
  return sum;
}

static bool perlin_noise_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  if(slot.ext != stb_perlin_symbol) {
    return false;
  }

  Symbol type(slot.parameter("type"));
  uint32_t size(512), octaves(1), wrap(256), span(1);
  float gain(.5f), lacunarity(2.f);

  slot.parameter("size", size);
  slot.parameter("octaves", octaves);
  slot.parameter("wrap", wrap);
  slot.parameter("span", span);
  slot.parameter("gain", gain);
  slot.parameter("persistence", gain);
  slot.parameter("lacunarity", lacunarity);

  slot.persistent = true;
  intermediate.width = size;
  intermediate.height = size;
  intermediate.format = VK_FORMAT_R8_UNORM;
  intermediate.binary = Buffer(size*size);

  const float mult = float(span) / size;

  if(type == ridge_symbol) {
    for(uint32_t x(0); x < size; x++) {
      for(uint32_t y(0); y < size; y++) {
        const float noise = ridge(x * mult, y * mult, lacunarity, gain, octaves, wrap);
        ((uint8_t*)intermediate.binary.data())[x*size + y] = uint8_t(noise * 255.f);
      }
    }
  } else {
    for(uint32_t x(0); x < size; x++) {
      for(uint32_t y(0); y < size; y++) {
        const float noise = turbulence(x * mult, y * mult, lacunarity, gain, octaves, wrap);
        ((uint8_t*)intermediate.binary.data())[x*size + y] = uint8_t(noise * 255.f);
      }
    }
  }

  return true;
}

void stb_perlin_module_init() {
  ResourceLoading<Texture>::add_loader(perlin_noise_loader);
}
