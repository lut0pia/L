#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

int main(int argc,const char* argv[]) {
  new STB();
  new OBJ();
  TypeInit();
  Window::openFullscreen("Test",Window::opengl|Window::nocursor);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  Engine::addUpdate<SpriteAnimator>();
  Engine::addUpdate<ScriptComponent>();
  Engine::addUpdate<RigidBody>();
  Engine::addUpdate<Collider>();
  Engine::addRender<Sprite>();
  Engine::addRender<StaticMesh>();
  Engine::addRender<Collider>();
  {
    ScriptComponent::init();
    FileStream file("startup.ls","rb");
    if(file){
      Script::Context startupContext;
      startupContext.read(file);
    } else out << "Couldn't open file startup.ls\n";
  }
  while(Window::loop()) {
    Window::Event e;
    while(Window::newEvent(e)) {}
    Engine::update();
  }
  return 0;
}
