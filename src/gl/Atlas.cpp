#include "Atlas.h"

#include "../image/Bitmap.h"

using namespace L;
using namespace GL;

Atlas::Atlas(GLsizei width, GLsizei height)
  : Texture(width, height), _xs{0}, _ys{0} {
  parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
Interval2f Atlas::add(const Bitmap& bmp) {
  Vector2i i(0, 0);
  while(true) {
    const Vector2i pos(_xs[i.x()], _ys[i.y()]);
    const Interval2i candidate(pos, pos+Vector2i(bmp.width(), bmp.height()));
    const bool valid(candidate.max().x()<width() && candidate.max().y()<height() && !_parts.overlaps(candidate));

    if(valid) {
      subload(bmp, candidate.min().x(), candidate.min().y());
      _parts.insert(candidate, true);
      _xs.insert(candidate.max().x());
      _ys.insert(candidate.max().y());
      return pixelToCoords(candidate);
    } else if(candidate.max().y()>=height())
      break;
    else {
      if(++i.x()==_xs.size()) {
        i.x() = 0;
        if(++i.y()==_ys.size())
          break; // No more y steps
      }
    }
  }
  return Interval2f();
}
Interval2f Atlas::pixelToCoords(const Interval2i& i) const {
  const Vector2f ratio(1.f/width(), 1.f/height());
  return Interval2f(ratio*i.min(), ratio*i.max());
}
