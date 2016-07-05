#include "Engine.h"

#include "Camera.h"
#include "../hash.h"
#include "../gl/Buffer.h"
#include "../gl/GL.h"
#include "../gl/Program.h"
#include "../system/Window.h"

using namespace L;

Set<void(*)()> Engine::_updates;
Set<void(*)(const Camera&)> Engine::_renders;
Map<uint32_t,Ref<GL::Texture> > Engine::_textures;
Map<uint32_t,Ref<GL::Mesh> > Engine::_meshes;
Timer Engine::_timer;
Time Engine::_deltaTime;
float Engine::_deltaSeconds,Engine::_fps,Engine::_timescale(1.f);
uint32_t Engine::_frame(0);

void Engine::update() {
  _deltaTime = _timer.frame();
  _fps = 1.f/_deltaTime.fSeconds();
  _deltaTime = min(_deltaTime,Time(0,50))*_timescale; // Delta time shouldn't be over 50ms
  _deltaSeconds = _deltaTime.fSeconds();
  for(auto&& update : _updates)
    update();
  for(auto&& camera : Pool<Camera>::global){
    camera.prerender();
    for(auto&& render : _renders)
      render(camera);
    camera.postrender();
  }
  _frame++;
}

GL::Buffer& Engine::sharedUniform(){
  static GL::Buffer u(GL_UNIFORM_BUFFER,sizeof(Matrix44f),nullptr,GL_DYNAMIC_DRAW,0);
  return u;
}

const Ref<GL::Texture>& Engine::texture(const char* fp) {
  String filepath(fp);
  if(_textures.has(fnv1a(filepath)))
    return _textures[fnv1a(filepath)];
  else return _textures[fnv1a(filepath)] = ref<GL::Texture>(Bitmap(filepath));
}
const Ref<GL::Mesh>& Engine::mesh(const char* fp) {
  String filepath(fp);
  if(_meshes.has(fnv1a(filepath)))
    return _meshes[fnv1a(filepath)];
  else return _meshes[fnv1a(filepath)] = ref<GL::Mesh>(filepath);
}
