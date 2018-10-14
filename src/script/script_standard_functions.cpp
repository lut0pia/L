#include "ScriptContext.h"

#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "../engine/Resource.inl"
#include "../image/Color.h"
#include "../macros.h"
#include "../math/Rand.h"
#include "../stream/CFileStream.h"
#include "../math/Vector.h"
#include "../system/Device.h"
#include "../system/Window.h"
#include "../time/Time.h"

using namespace L;

#define L_SCRIPT_NATIVE(name,...) \
  ScriptGlobal L_CONCAT(native_,__LINE__)(name,(ScriptNativeFunction)(__VA_ARGS__))
#define L_SCRIPT_NATIVE_RETURN(name,p_count,...) \
  L_SCRIPT_NATIVE(name,[](ScriptContext& c) { L_ASSERT(c.param_count()==p_count); c.return_value() = __VA_ARGS__; })

L_SCRIPT_NATIVE_RETURN("rand", 0, Rand::nextFloat());
L_SCRIPT_NATIVE_RETURN("now", 0, Time::now());
L_SCRIPT_NATIVE_RETURN("window-width", 0, Window::width());
L_SCRIPT_NATIVE_RETURN("window-height", 0, Window::height());
L_SCRIPT_NATIVE_RETURN("mouse-x", 0, Window::mouse_x());
L_SCRIPT_NATIVE_RETURN("mouse-y", 0, Window::mouse_y());
L_SCRIPT_NATIVE_RETURN("non-null", 1, !c.param(0).is<void>() && c.param(0).as<void*>()!=nullptr);
L_SCRIPT_NATIVE_RETURN("clamp", 3, clamp(c.param(0), c.param(1), c.param(2)));
L_SCRIPT_NATIVE_RETURN("typename", 1, c.param(0).type()->name);
L_SCRIPT_NATIVE_RETURN("time", 1, Time(c.param(0).get<float>()*1000000.f));
L_SCRIPT_NATIVE_RETURN("button-pressed", 1, Device::any_button(Device::symbol_to_button(c.param(0))));
L_SCRIPT_NATIVE_RETURN("normalize", 1, c.param(0).get<Vector3f>().normalized());
L_SCRIPT_NATIVE_RETURN("cross", 2, c.param(0).get<Vector3f>().cross(c.param(1).get<Vector3f>()));
L_SCRIPT_NATIVE_RETURN("length", 1, c.param(0).get<Vector3f>().length());
L_SCRIPT_NATIVE_RETURN("distance", 2, c.param(0).get<Vector3f>().dist(c.param(1).get<Vector3f>()));
L_SCRIPT_NATIVE_RETURN("dot", 2, c.param(0).get<Vector3f>().dot(c.param(1).get<Vector3f>()));
L_SCRIPT_NATIVE_RETURN("sqrt", 1, sqrtf(c.param(0)));
L_SCRIPT_NATIVE_RETURN("pow", 2, powf(c.param(0), c.param(1)));
L_SCRIPT_NATIVE_RETURN("sin", 1, sinf(c.param(0)));
L_SCRIPT_NATIVE_RETURN("cos", 1, cosf(c.param(0)));
L_SCRIPT_NATIVE_RETURN("tan", 1, tanf(c.param(0)));
L_SCRIPT_NATIVE_RETURN("abs", 1, abs(c.param(0).get<float>()));
L_SCRIPT_NATIVE_RETURN("floor", 1, floorf(c.param(0)));
L_SCRIPT_NATIVE_RETURN("ceil", 1, ceilf(c.param(0)));

L_SCRIPT_NATIVE("max",[](ScriptContext& c) {
  L_ASSERT(c.param_count()>=1);
  c.return_value() = c.param(0);
  for(uintptr_t i(1); i<c.param_count(); i++)
    if(c.param(i)>c.return_value())
      c.return_value() = c.param(i);
});
L_SCRIPT_NATIVE("min",[](ScriptContext& c) {
  L_ASSERT(c.param_count()>=1);
  c.return_value() = c.param(0);
  for(uintptr_t i(1); i<c.param_count(); i++)
    if(c.param(i)<c.return_value())
      c.return_value() = c.param(i);
});
L_SCRIPT_NATIVE("print",[](ScriptContext& c) {
  for(uintptr_t i(0); i<c.param_count(); i++)
    out << c.param(i);
});
L_SCRIPT_NATIVE("break",[](ScriptContext& c) {
  debugbreak();
});
L_SCRIPT_NATIVE("vec",[](ScriptContext& c) {
  const uint32_t param_count(c.param_count());
  Vector3f& vector(c.return_value().make<Vector3f>());
  if(param_count)
    for(uint32_t i(0); i<3; i++)
      vector[i] = c.param(min(param_count-1, i));
  else vector = 0.f;
});
L_SCRIPT_NATIVE("vec4",[](ScriptContext& c) {
  const uint32_t param_count(c.param_count());
  Vector4f& vector(c.return_value().make<Vector4f>());
  if(param_count)
    for(uint32_t i(0); i<4; i++)
      vector[i] = c.param(min(param_count-1, i));
  else vector = 0.f;
});
L_SCRIPT_NATIVE("lerp",[](ScriptContext& c) {
  L_ASSERT(c.param_count()==3);
  const float a(c.param(0).get<float>()), b(c.param(1).get<float>()), alpha(c.param(2).get<float>());
  c.return_value() = (a*(1.f-alpha)+b*alpha);
});
L_SCRIPT_NATIVE("color",[](ScriptContext& c) {
  if(c.param(0).is<String>()) {
    c.return_value() = Color(c.param(0).as<String>());
  } else {
    Color& color(c.return_value().make<Color>() = Color::white);
    const uint32_t params(min(c.param_count(), 4u));
    for(uint32_t i(0); i<params; i++)
      color[i] = c.param(i).is<float>() ? (c.param(i).as<float>()*255) : c.param(i).get<int>();
  }
});
L_SCRIPT_NATIVE("left-pad",[](ScriptContext& c) {
  L_ASSERT(c.param_count()==3);
  c.return_value() = c.param(0).get<String>();
  String& str(c.return_value().as<String>());
  const uint32_t wanted_size(c.param(1).get<float>());
  const String append(c.param(2).get<String>());
  while(str.size()<wanted_size) {
    str = append + str;
  }
});
L_SCRIPT_NATIVE("count",[](ScriptContext& c) {
  L_ASSERT(c.param_count()==1);
  if(c.param(0).is<Ref<Table<Var, Var>>>())
    c.return_value() = float(c.param(0).as<Ref<Table<Var, Var>>>()->count());
  else c.return_value() = 0;
});

template<class T>
class ScriptTypeValue {
public:
  ScriptTypeValue(const Symbol& name, const Var& value) {
    ScriptContext::type_value(Type<T>::description(), name) = value;
  }
};

#define L_SCRIPT_NATIVE_METHOD(type,name,...) \
  ScriptTypeValue<type> L_CONCAT(native_method_,__LINE__)(name,(ScriptNativeFunction)(__VA_ARGS__));
#define L_SCRIPT_NATIVE_ACCESS_METHOD(type,name) \
  L_SCRIPT_NATIVE_METHOD(type,#name,[](ScriptContext& c) { \
    L_ASSERT(c.param_count()==0 && c.current_self().is<type>()); \
    c.return_value() = c.current_self().as<type>().name(); \
  })

L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, x);
L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, y);
L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, z);
