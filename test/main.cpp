#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

int main(int argc,const char* argv[]) {
  new STB();
  new OBJ();
  TypeInit();
  Window::openFullscreen("Test",Window::nocursor|Window::loopcursor);
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
  float bayer[] = {
    0,12,3,15,
    8,4,11,7,
    2,14,1,13,
    10,6,9,5
  };
  for(auto&& b : bayer) b = (1+b)/(4*4+1);
  Engine::ditherMatrix(bayer,4,4);
  while(Window::loop())
    Engine::update();
  Engine::clear();
  return 0;
}
