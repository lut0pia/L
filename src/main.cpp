#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

int main(int argc,const char* argv[]) {
  new STB();
  new OBJ();
  TypeInit();
  Window::openFullscreen("Sample",Window::nocursor|Window::loopcursor);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  Engine::addUpdate<SpriteAnimator>();
  Engine::addUpdate<ScriptComponent>();
  Engine::addUpdate<RigidBody>();
  Engine::addUpdate<Collider>();
  Engine::addRender<Primitive>();
  Engine::addRender<Sprite>();
  Engine::addRender<StaticMesh>();
  Engine::addEvent<ScriptComponent>();
#ifdef L_DEBUG
  Engine::addRender<Collider>();
#endif
  {
    ScriptComponent::init();
    FileStream file("startup.ls","rb");
    if(file){
      Script::Context startupContext;
      startupContext.read(file);
    } else out << "Couldn't open file startup.ls\n";
  }
  const uint32_t bs(32);
  float bayer[bs*bs];
  Engine::ditherMatrix(bayerMatrix(bs,bs,bayer),bs,bs);
  while(Window::loop())
    Engine::update();
  Engine::clear();
  return 0;
}
