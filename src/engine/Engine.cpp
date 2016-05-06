#include "Engine.h"

#include "Camera.h"
#include "../hash.h"

using namespace L;

Set<void (*)()> Engine::_updates;
Set<void (*)(const Camera&)> Engine::_renders;
Map<uint32_t,Ref<GL::Texture> > Engine::_textures;
Map<uint32_t,Ref<GL::Mesh> > Engine::_meshes;
Timer Engine::_timer;
Time Engine::_deltaTime;
float Engine::_deltaSeconds, Engine::_fps;
uint32_t Engine::_frame(0);

void Engine::update() {
  _deltaTime = _timer.frame();
  _deltaSeconds = _deltaTime.fSeconds();
  _fps = 1.f/_deltaSeconds;
  for(auto&& update : _updates)
    update();
  for(auto&& camera : Pool<Camera>::global){
    camera.prerender();
    for(auto&& render : _renders)
      render(camera);
  }
  _frame++;
}
const Ref<GL::Texture>& Engine::texture(const char* fp) {
  String filepath(fp);
  if(_textures.has(fnv1a(filepath)))
    return _textures[fnv1a(filepath)];
  else return _textures[fnv1a(filepath)] = new GL::Texture(Bitmap(filepath));
}
const Ref<GL::Mesh>& Engine::mesh(const char* fp) {
  String filepath(fp);
  if(_meshes.has(fnv1a(filepath)))
    return _meshes[fnv1a(filepath)];
  else return _meshes[fnv1a(filepath)] = new GL::Mesh(filepath);
}
