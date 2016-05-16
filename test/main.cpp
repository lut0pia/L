#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

int main(int argc,const char* argv[]) {
  new STB();
  new OBJ();
  TypeInit();
  Window::openFullscreen("Test",Window::opengl|Window::nocursor);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  Engine::addRender<Sprite>();
  Engine::addRender<StaticMesh>();
  Engine::addRender<Collider>();
  Engine::addUpdate<SpriteAnimator>();
  Engine::addUpdate<ScriptComponent>();
  Engine::addUpdate<RigidBody>();
  Engine::addUpdate<Collider>();
  {
    ScriptComponent::init();
    FileStream file("startup.ls","rb");
    Script::Context startupContext;
    startupContext.read(file);
  }
  while(Window::loop()) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    Window::Event e;
    while(Window::newEvent(e)) {}
    GL::drawAxes();
    Engine::update();
    Window::swapBuffers();
  }
  return 0;
}
