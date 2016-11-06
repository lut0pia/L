#pragma once

#include "Sprite.h"

namespace L {
  class SpriteAnimator : public Component {
      L_COMPONENT(SpriteAnimator)
    private:
      Sprite* _sprite;
      Vector2i _frames;
      Vector2f _frameSize;
      float _time, _nextFrame;
      int _frame;
    public:
      inline SpriteAnimator() : _time(0),_nextFrame(0),_frame(0){}
      void updateComponents() {
        _sprite = entity()->requireComponent<Sprite>();
      }
      void update() {
        _time += Engine::deltaSeconds();
        if(_time>=_nextFrame) {
          frame(_frame++);
          _nextFrame = .1f;
          _time = 0;
        }
      }
      void frame(int i) {
        i = i%_frames.product(); // Debug only
        frame(Vector2i(i%_frames.x(),i/_frames.x()));
      }
      void frame(const Vector2i& f) {
        _sprite->uv(Interval2f(_frameSize*f,_frameSize*f+_frameSize));
      }
      void load(const char* filename, const Vector2i& frames) {
        _sprite->texture(filename);
        _frames = frames;
        _frameSize.x() = 1.f/frames.x();
        _frameSize.y() = 1.f/frames.y();
      }
  };
}
