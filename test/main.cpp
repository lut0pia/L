#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

class Lock {
  private:
    int _lock;
  public:
    Lock() :_lock(0) {}
    inline bool lock() {
      _asm {
        mov eax,0
        mov ecx,this
        mov edx,1
        lock cmpxchg[ecx],edx
        mov eax,0
        setz al
      }
    }
    inline void unlock() { _lock = 0; }
};
void test(int prout) {
  out << "arg: " << prout << '\n';
}
void test2(Fiber* p) {
  out << "ok " << (int)p->param() <<  '\n';
}
int main(int argc,const char* argv[]) {
  new STB();
  new OBJ();
  TypeInit();
  /*
  Vector3f* v = create<Vector3f>(1,1,1);
  out << v << " -> " << *v << '\n';
  destroy(v);
  */
  /*
  Script::Context c;
  c.read(FileStream("lol.ls","rb"));
  */
  /*
  Fiber* fiber(Fiber::convert());
  for(int i(0); i<128; i++)
  Fiber::create(test2,(void*)i);
  fiber->yield();
  //Fiber a();
  //  Fiber f(test2);
  //f.switchTo();
  */
  /*
  Assembly a;
  a.load(test);
  out << a << '\n';
  //return 0;
  a.clear();
  a.mov(Assembly::ebx,32);
  uint32 startLabel(a.label());
  a.push(Assembly::ebx);
  a.include(test);
  a.pop();
  a.dec(Assembly::ebx);
  a.clr(Assembly::eax);
  a.cmp(Assembly::ebx,Assembly::eax);
  a.jg(startLabel);
  a.mov(Assembly::eax,Assembly::ebx);
  a.retn();
  out << a << '\n';
  typedef int(*JittedFunc)(...);
  JittedFunc f((JittedFunc)a.commit());
  out << f(4) << '\n';
  */
  /*
  Timer timer;
  float fl(2);
  Fixed f(2);
  uint n(0xffffff);
  timer.setoff();
  for(uint i(0); i<n; i++)
  fl *= Rand::next<float>();
  out << timer.since().microseconds() << '\n';
  timer.setoff();
  for(uint i(0); i<n; i++)
  f *= Rand::next<Fixed>();
  out << timer.since().microseconds() << '\n';
  out << fl << f << '\r' << "                                        ";
  */
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
  // Entities
  Entity *camEntity(new Entity());
  Transform* camTrans(camEntity->add<Transform>());
  Camera* camera(camEntity->add<Camera>());
  camEntity->add<ScriptComponent>()->load("test.ls");
  camera->perspective(80,16.f/9.f,.1f,512.f);
  camTrans->move(Vector3f(0,-8.f,0));
  /*
  Entity *a(new Entity());
  Transform* objTrans(a->add<Transform>());
  objTrans->move(Vector3f(-2,0,0));
  a->add<Sprite>()->vertex(Interval2f(Vector2f(-2,-2),Vector2f(2,2)));
  a->add<SpriteAnimator>()->load("spritesheet.png",Vector2i(3,3));
  Entity *c(new Entity(a));
  c->component<Transform>()->move(Vector3f(4,0,0));
  //c->add<StaticMesh>()->mesh("");
  */
  Entity *d(new Entity());
  d->add<Transform>()->move(Vector3f(0,0,4.f));
  d->add<RigidBody>();
  d->add<Collider>()->box(Interval3f(Vector3f(-1,-1,-1),Vector3f(1,1,1)));
  d->component<Transform>()->rotate(Vector3f::random(),Rand::next(.0f,PI<float>()));
  Entity *e(new Entity());
  e->add<Transform>()->move(Vector3f(0,0,-4.f));
  e->add<Collider>()->box(Interval3f(Vector3f(-8,-8,-.5f),Vector3f(8,8,.5f)));
  e->component<Transform>()->rotate(Vector3f(0,1,0),-.5f);
  Entity *f(new Entity(d));
  f->component<Transform>()->moveAbsolute(Vector3f(0,0,4.f));
  Entity *g(new Entity(e));
  e->component<Transform>()->moveAbsolute(Vector3f(-4.f,0,0));
  e->component<Transform>()->rotate(Vector3f(0,1,0),1.f);
  /*
  GL::Mesh mesh;
  GL::makeSphere(mesh,1);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  mesh.draw();
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  */
  while(Window::loop()) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    Window::Event e;
    while(Window::newEvent(e)) {}
    camTrans->thetaLook(-(Window::mousePosition().x()-800)*Engine::deltaSeconds()*.2f);
    camTrans->phiLook(-(Window::mousePosition().y()-450)*Engine::deltaSeconds()*.2f);
    if(Window::isPressed(Window::Event::Z)) camTrans->move(Vector3f(0,Engine::deltaSeconds()*4.f,0));
    if(Window::isPressed(Window::Event::S)) camTrans->move(Vector3f(0,-Engine::deltaSeconds()*4.f,0));
    if(Window::isPressed(Window::Event::D)) camTrans->move(Vector3f(Engine::deltaSeconds()*4.f,0,0));
    if(Window::isPressed(Window::Event::Q)) camTrans->move(Vector3f(-Engine::deltaSeconds()*4.f,0,0));
    SetCursorPos(800,450);
    GL::drawAxes();
    Engine::update();
    Window::swapBuffers();
  }
  //Ref<Regex::Base> regex(Regex::from("\\-?[0-9]+(\\.[0-9]+)?|\".*\"|\\{.*\\}|true|false|null"));
  /*
  Ref<Regex::Base> regex(Regex::from("a+"));
  String s;
  while(true) {
  s = in.line();
  if(s=="exit")   break;
  else            out << s << ": " << ((regex->matches(s)) ? "true" : "false") << '\n';
  }
  */
  return 0;
}
