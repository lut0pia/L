#include "ScriptContext.h"

#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "../engine/Resource.inl"
#include "../macros.h"
#include "../math/geometry.h"
#include "../math/Rand.h"
#include "../rendering/Color.h"
#include "../stream/CFileStream.h"
#include "../math/Vector.h"
#include "../system/Device.h"
#include "../system/Window.h"
#include "../time/Time.h"

using namespace L;

#define L_SCRIPT_NATIVE(name,...) \
  ScriptGlobal(name).value() = (ScriptNativeFunction)(__VA_ARGS__)
#define L_SCRIPT_NATIVE_RETURN(name,p_count,...) \
  L_SCRIPT_NATIVE(name,[](ScriptContext& c) { L_ASSERT(c.param_count()==p_count); c.return_value() = __VA_ARGS__; })

#define L_SCRIPT_NATIVE_METHOD(type,name,...) \
  ScriptContext::type_value(Type<type>::description(), name) = (ScriptNativeFunction)(__VA_ARGS__);
#define L_SCRIPT_NATIVE_ACCESS_METHOD(type,name) \
  L_SCRIPT_NATIVE_METHOD(type,#name,[](ScriptContext& c) { \
    L_ASSERT(c.param_count()==0 && c.current_self().is<type>()); \
    c.return_value() = c.current_self().as<type>().name(); \
  })

template <class T>
static void register_script_method(const char* name, ScriptNativeFunction func) {
  ScriptContext::type_value(Type<T>::description(), Symbol(name)) = func;
}

void L::init_script_standard_functions() {
  L_SCRIPT_NATIVE_RETURN("rand", 0, Rand::nextFloat());
  L_SCRIPT_NATIVE_RETURN("now", 0, Time::now());
  L_SCRIPT_NATIVE_RETURN("window_width", 0, float(Window::width()));
  L_SCRIPT_NATIVE_RETURN("window_height", 0, float(Window::height()));
  L_SCRIPT_NATIVE_RETURN("mouse_x", 0, float(Window::cursor_x()));
  L_SCRIPT_NATIVE_RETURN("mouse_y", 0, float(Window::cursor_y()));
  L_SCRIPT_NATIVE_RETURN("non_null", 1, !c.param(0).is<void>() && c.param(0).as<void*>() != nullptr);
  L_SCRIPT_NATIVE_RETURN("clamp", 3, clamp(c.param(0), c.param(1), c.param(2)));
  L_SCRIPT_NATIVE_RETURN("typename", 1, c.param(0).type()->name);
  L_SCRIPT_NATIVE_RETURN("time", 1, Time(int64_t(c.param(0).get<float>() * 1000000.f)));
  L_SCRIPT_NATIVE_RETURN("button_pressed", 1, Device::any_button(Device::symbol_to_button(c.param(0))));
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

  L_SCRIPT_NATIVE_RETURN("euler_degrees", 3, euler_degrees(c.param(0), c.param(1), c.param(2)));
  L_SCRIPT_NATIVE_RETURN("euler_radians", 3, euler_radians(c.param(0), c.param(1), c.param(2)));

  L_SCRIPT_NATIVE("max", [](ScriptContext& c) {
    L_ASSERT(c.param_count() >= 1);
    c.return_value() = c.param(0);
    for(uintptr_t i(1); i < c.param_count(); i++)
      if(c.param(i) > c.return_value())
        c.return_value() = c.param(i);
  });
  L_SCRIPT_NATIVE("min", [](ScriptContext& c) {
    L_ASSERT(c.param_count() >= 1);
    c.return_value() = c.param(0);
    for(uintptr_t i(1); i < c.param_count(); i++)
      if(c.param(i) < c.return_value())
        c.return_value() = c.param(i);
  });
  L_SCRIPT_NATIVE("print", [](ScriptContext& c) {
    for(uintptr_t i(0); i < c.param_count(); i++)
      out << c.param(i);
  });
  L_SCRIPT_NATIVE("break", [](ScriptContext&) {
    debugbreak();
  });
  L_SCRIPT_NATIVE("vec", [](ScriptContext& c) {
    const uint32_t param_count(c.param_count());
    Vector3f& vector(c.return_value().make<Vector3f>());
    if(param_count)
      for(uint32_t i(0); i < 3; i++)
        vector[i] = c.param(min(param_count - 1, i));
    else vector = 0.f;
  });
  L_SCRIPT_NATIVE("vec4", [](ScriptContext& c) {
    const uint32_t param_count(c.param_count());
    Vector4f& vector(c.return_value().make<Vector4f>());
    if(param_count)
      for(uint32_t i(0); i < 4; i++)
        vector[i] = c.param(min(param_count - 1, i));
    else vector = 0.f;
  });
  L_SCRIPT_NATIVE("lerp", [](ScriptContext& c) {
    L_ASSERT(c.param_count() == 3);
    const float a(c.param(0).get<float>()), b(c.param(1).get<float>()), alpha(c.param(2).get<float>());
    c.return_value() = (a * (1.f - alpha) + b * alpha);
  });
  L_SCRIPT_NATIVE("color", [](ScriptContext& c) {
    if(c.param(0).is<String>()) {
      c.return_value() = Color(c.param(0).as<String>());
    } else {
      Color& color(c.return_value().make<Color>() = Color::white);
      const uint32_t params(min(c.param_count(), 4u));
      for(uint32_t i(0); i < params; i++)
        color[i] = uint8_t(c.param(i).is<float>() ? (c.param(i).as<float>() * 255) : c.param(i).get<int>());
    }
  });
  L_SCRIPT_NATIVE("left_pad", [](ScriptContext& c) {
    L_ASSERT(c.param_count() == 3);
    c.return_value() = c.param(0).get<String>();
    String& str(c.return_value().as<String>());
    const uint32_t wanted_size(uint32_t(c.param(1).get<float>()));
    const String append(c.param(2).get<String>());
    while(str.size() < wanted_size) {
      str = append + str;
    }
  });
  L_SCRIPT_NATIVE("count", [](ScriptContext& c) {
    L_ASSERT(c.param_count() == 1);
    if(Ref<Table<Var, Var>>* table = c.param(0).try_as<Ref<Table<Var, Var>>>()) {
      c.return_value() = float((*table)->count());
    } else if(Ref<Array<Var>>* array = c.param(0).try_as<Ref<Array<Var>>>()) {
      c.return_value() = float((*array)->size());
    } else {
      c.return_value() = 0;
    }
  });

  L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, x);
  L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, y);
  L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, z);

  Type<Ref<Table<Var, Var>>>::cancmp<>();
  Type<Ref<Array<Var>>>::cancmp<>();

  register_script_method<Ref<Array<Var>>>("push", [](ScriptContext& c) {
    if(Ref<Array<Var>>* array = c.current_self().try_as<Ref<Array<Var>>>()) {
      for(uint32_t i = 0; i < c.param_count(); i++) {
        (*array)->push(c.param(i));
      }
    }
  });
  register_script_method<Ref<Array<Var>>>("shift", [](ScriptContext& c) {
    if(Ref<Array<Var>>* array = c.current_self().try_as<Ref<Array<Var>>>()) {
      if((*array)->size() > 0) {
        c.return_value() = (*array)[0];
        (*array)->erase(0);
      }
    }
  });
  register_script_method<Ref<Array<Var>>>("contains", [](ScriptContext& c) {
    if(Ref<Array<Var>>* array = c.current_self().try_as<Ref<Array<Var>>>()) {
      if(c.param_count() > 0) {
        const Var& search = c.param(0);
        c.return_value() = false;
        for(const Var& v : **array) {
          if(v == search) {
            c.return_value() = true;
            return;
          }
        }
      }
    }
  });
}