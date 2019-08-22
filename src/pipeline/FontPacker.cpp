#include "FontPacker.h"

#include "../math/math.h"

using namespace L;

FontPacker::FontPacker(Font::Intermediate& intermediate) : _intermediate(intermediate) {
  _xs.insert(0);
  _ys.insert(0);
}
void FontPacker::grow() {
  const uint32_t old_size(_intermediate.texture_intermediate.width);
  const uint32_t new_size(max(old_size, 8u)*2);
  Buffer& old_binary(_intermediate.texture_intermediate.binary);
  Buffer new_binary(new_size*new_size);
  memset(new_binary.data(), 0xff, new_binary.size()); // Max distance
  if(old_size) { // Copy old values
    for(uint32_t row(0); row<old_size; row++) {
      memcpy((uint8_t*)new_binary.data()+new_size*row, (uint8_t*)old_binary.data()+old_size*row, old_size);
    }
    for(auto& pair : _intermediate.glyphs) { // Adjust previous coordinates
      pair.value().atlas_coords *= 0.5f;
    }
  } else { // First ever allocation
    _intermediate.texture_intermediate.format = VK_FORMAT_R8_UNORM;
  }
  swap(old_binary, new_binary);
  _intermediate.texture_intermediate.width =
    _intermediate.texture_intermediate.height = new_size;
}
Interval2i FontPacker::add_bmp(const uint8_t* data, uint32_t width, uint32_t height, uint32_t hash) {
  Vector2i i(0, 0);
  while(true) {
    const Vector2i pos(_xs[i.x()], _ys[i.y()]);
    const Interval2i candidate(pos, pos+Vector2i(width, height));
    const bool valid(candidate.max().x() <= int(_intermediate.texture_intermediate.width) &&
      candidate.max().y() <= int(_intermediate.texture_intermediate.height) &&
      !_parts.overlaps(candidate));

    if(valid) {
      // Copy bitmap
      {
        const uint32_t atlas_size(_intermediate.texture_intermediate.width);
        for(uint32_t row(0); row<height; row++) {
          memcpy((uint8_t*)_intermediate.texture_intermediate.binary.data()+atlas_size*(row+pos.y())+pos.x(), data+width*row, width);
        }
      }
      _parts.insert(candidate, true);
      _hash_to_part[hash] = candidate;
      _xs.insert(candidate.max().x());
      _ys.insert(candidate.max().y());
      return candidate;
    } else if(uint32_t(candidate.max().y())>_intermediate.texture_intermediate.height) {
      grow();
      i = {0,0};
    } else {
      if(++i.x()==_xs.size()) {
        i.x() = 0;
        if(++i.y()==_ys.size())
          break; // No more y steps
      }
    }
  }
  error("Couldn't add bitmap to font atlas, that's not supposed to happen!");
  return Interval2i();
}
Interval2f FontPacker::pixel_to_coords(const Interval2i& i) const {
  const Vector2f ratio(1.f/_intermediate.texture_intermediate.width, 1.f/_intermediate.texture_intermediate.height);
  return Interval2f(ratio*i.min(), ratio*i.max());
}

static bool value_at(const uint8_t* bmp, size_t width, size_t height, float x, float y) {
  const intptr_t xi(intptr_t(roundf(x)));
  const intptr_t yi(intptr_t(roundf(y)));
  if(xi<0 || yi<0 || xi>=intptr_t(width) || yi>=intptr_t(height)) {
    return false;
  } else {
    return bmp[yi*width+xi]>=128;
  }
}
static inline bool distance_helper(const uint8_t* bmp, size_t width, size_t height, bool origin_value, float ox, float oy, float x, float y, float& min_distance) {
  const bool value(value_at(bmp, width, height, x, y));
  if(value!=origin_value) {
    const float dx(x-ox), dy(y-oy);
    const float distance(dx*dx+dy*dy);
    if(distance<min_distance) {
      min_distance = distance;
      return true;
    }
  }
  return false;
}
static float distance_to_invert(const uint8_t* bmp, size_t width, size_t height, float ox, float oy, Vector2f step) {
  const bool origin_value(value_at(bmp, width, height, ox, oy));
  float min_distance(FLT_MAX);
  // Sweep from origin and stop when no longer relevant
  intptr_t max_i(-1);
  bool stop(false);
  for(intptr_t i(1); !stop; i++) {
    for(intptr_t j(-i); !stop && j<=i; j++) {
      const bool found(
        distance_helper(bmp, width, height, origin_value, ox, oy, ox+j*step.x(), oy-i*step.y(), min_distance) ||
        distance_helper(bmp, width, height, origin_value, ox, oy, ox+j*step.x(), oy+i*step.y(), min_distance) ||
        distance_helper(bmp, width, height, origin_value, ox, oy, ox-i*step.x(), oy+j*step.y(), min_distance) ||
        distance_helper(bmp, width, height, origin_value, ox, oy, ox+i*step.x(), oy+j*step.y(), min_distance));
      if(found && max_i<0) {
        max_i = intptr_t(ceilf(sqrtf(float(i*i + i*i))));
      }
      if(max_i>=0 && i>=max_i) {
        stop = true;
      }
    }
  }
  return sqrt(min_distance)*(origin_value?-1.f:1.f);
}
void FontPacker::add_glyph(const uint8_t* bmp, size_t width, size_t height, Font::Glyph& glyph) {
  { // Check visibility
    bool visible(false);
    for(uintptr_t x(0); x<width && !visible; x++) {
      for(uintptr_t y(0); y<height && !visible; y++) {
        if(value_at(bmp, width, height, float(x), float(y))) {
          visible = true;
        }
      }
    }
    if(!visible) {
      glyph.size = 0.f; // Means no rendering necessary
      return;
    }
  }

  const Vector2f bmp_pixels_per_unit(Vector2f(float(width), float(height))/glyph.size);

  // Add padding to avoid sampling neighbor glyphs in shader
  const float font_unit_padding(0.2f);
  glyph.origin -= font_unit_padding;
  glyph.size += font_unit_padding*2.f;

  // Use the hash to find a potential existing glyph in the atlas
  const uint32_t hash(fnv1a((const char*)bmp, width*height));
  if(const Interval2i* pixel_interval = _hash_to_part.find(hash)) {
    glyph.atlas_coords = pixel_to_coords(*pixel_interval);
    return;
  }

  const float sdf_pixels_per_unit(32.f);
  const Vector2i sdf_pixel_size(int(ceilf(glyph.size.x()*sdf_pixels_per_unit)), int(ceilf(glyph.size.y()*sdf_pixels_per_unit)));
  const size_t sdf_byte_size(sdf_pixel_size.product());
  uint8_t* sdf((uint8_t*)Memory::alloc(sdf_byte_size));
  const Vector2f step(width / max<float>(float(sdf_pixel_size.x()), float(width)), height / max<float>(float(sdf_pixel_size.y()), float(height)));
  const float pixel_distance_to_half_byte(128.f/bmp_pixels_per_unit.x()); // TODO: this isn't very robust

  for(intptr_t x(0); x<sdf_pixel_size.x(); x++) {
    for(intptr_t y(0); y<sdf_pixel_size.y(); y++) {
      const float font_unit_x((float(x)/sdf_pixels_per_unit)-font_unit_padding);
      const float font_unit_y((float(y)/sdf_pixels_per_unit)-font_unit_padding);
      const float xf(font_unit_x*bmp_pixels_per_unit.x()), yf(font_unit_y*bmp_pixels_per_unit.y());
      const uint8_t half_byte_distance(uint8_t(clamp<int32_t>(int32_t(distance_to_invert(bmp, width, height, xf, yf, step)*pixel_distance_to_half_byte), -128, 127)));
      sdf[y*sdf_pixel_size.x()+x] = 0x80+half_byte_distance;
    }
  }
  const Interval2i pixel_interval(add_bmp(sdf, sdf_pixel_size.x(), sdf_pixel_size.y(), hash));
  glyph.atlas_coords = pixel_to_coords(pixel_interval);

  Memory::free(sdf, sdf_byte_size);
}
