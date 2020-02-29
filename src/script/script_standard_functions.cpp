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
  ScriptContext::type_value(Type<type>::description(), Symbol(name)) = (ScriptNativeFunction)(__VA_ARGS__);
#define L_SCRIPT_NATIVE_ACCESS_METHOD(type,name) \
  L_SCRIPT_NATIVE_METHOD(type,#name,[](ScriptContext& c) { \
    L_ASSERT(c.param_count()==0 && c.current_self().is<type>()); \
    c.return_value() = c.current_self().as<type>().name(); \
  })

template <class T>
static void register_script_method(const char* name, ScriptNativeFunction func) {
  ScriptContext::type_value(Type<T>::description(), Symbol(name)) = func;
}
static void register_script_function(const char* name, ScriptNativeFunction func) {
  ScriptGlobal(Symbol(name)).value() = func;
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

  register_script_function("max", [](ScriptContext& c) {
    L_ASSERT(c.param_count() >= 1);
    c.return_value() = c.param(0);
    for(uintptr_t i(1); i < c.param_count(); i++)
      if(c.param(i) > c.return_value())
        c.return_value() = c.param(i);
  });
  register_script_function("min", [](ScriptContext& c) {
    L_ASSERT(c.param_count() >= 1);
    c.return_value() = c.param(0);
    for(uintptr_t i(1); i < c.param_count(); i++)
      if(c.param(i) < c.return_value())
        c.return_value() = c.param(i);
  });
  register_script_function("print", [](ScriptContext& c) {
    for(uintptr_t i(0); i < c.param_count(); i++)
      out << c.param(i);
  });
  register_script_function("break", [](ScriptContext&) {
    debugbreak();
  });
  register_script_function("lerp", [](ScriptContext& c) {
    L_ASSERT(c.param_count() == 3);
    const float a(c.param(0).get<float>()), b(c.param(1).get<float>()), alpha(c.param(2).get<float>());
    c.return_value() = (a * (1.f - alpha) + b * alpha);
  });
  register_script_function("color", [](ScriptContext& c) {
    if(c.param(0).is<String>()) {
      c.return_value() = Color(c.param(0).as<String>());
    } else {
      Color& color(c.return_value().make<Color>() = Color::white);
      const uint32_t params(min(c.param_count(), 4u));
      for(uint32_t i(0); i < params; i++)
        color[i] = uint8_t(c.param(i).is<float>() ? (c.param(i).as<float>() * 255) : c.param(i).get<int>());
    }
  });
  register_script_function("left_pad", [](ScriptContext& c) {
    L_ASSERT(c.param_count() == 3);
    c.return_value() = c.param(0).get<String>();
    String& str(c.return_value().as<String>());
    const uint32_t wanted_size(uint32_t(c.param(1).get<float>()));
    const String append(c.param(2).get<String>());
    while(str.size() < wanted_size) {
      str = append + str;
    }
  });
  register_script_function("count", [](ScriptContext& c) {
    L_ASSERT(c.param_count() == 1);
    if(Ref<Table<Var, Var>>* table = c.param(0).try_as<Ref<Table<Var, Var>>>()) {
      c.return_value() = float((*table)->count());
    } else if(Ref<Array<Var>>* array = c.param(0).try_as<Ref<Array<Var>>>()) {
      c.return_value() = float((*array)->size());
    } else {
      c.return_value() = 0;
    }
  });

  Type<Ref<Table<Var, Var>>>::cancmp<>();

  { // Array
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

  { // Vector
    register_script_function("vec", [](ScriptContext& c) {
      switch(c.param_count()) {
        case 2: c.return_value() = Vector2f(c.param(0).get<float>(), c.param(1).get<float>()); break;
        case 3: c.return_value() = Vector3f(c.param(0), c.param(1).get<float>(), c.param(2).get<float>()); break;
        case 4: c.return_value() = Vector4f(c.param(0), c.param(1).get<float>(), c.param(2).get<float>(), c.param(3).get<float>()); break;
        default: warning("Cannot create vector from %d components", c.param_count()); break;
      }
    });
    register_script_function("normalize", [](ScriptContext& c) {
      if(Vector2f* vec2 = c.param(0).try_as<Vector2f>()) {
        c.return_value() = vec2->normalized();
      } else if(Vector3f* vec3 = c.param(0).try_as<Vector3f>()) {
        c.return_value() = vec3->normalized();
      } else if(Vector4f* vec4 = c.param(0).try_as<Vector4f>()) {
        c.return_value() = vec4->normalized();
      }
    });
    register_script_function("length", [](ScriptContext& c) {
      if(Vector2f* vec2 = c.param(0).try_as<Vector2f>()) {
        c.return_value() = vec2->length();
      } else if(Vector3f* vec3 = c.param(0).try_as<Vector3f>()) {
        c.return_value() = vec3->length();
      } else if(Vector4f* vec4 = c.param(0).try_as<Vector4f>()) {
        c.return_value() = vec4->length();
      }
    });
    register_script_function("dist", [](ScriptContext& c) {
      if(Vector2f* vec2 = c.param(0).try_as<Vector2f>()) {
        c.return_value() = vec2->dist(c.param(1).get<Vector2f>());
      } else if(Vector3f* vec3 = c.param(0).try_as<Vector3f>()) {
        c.return_value() = vec3->dist(c.param(1).get<Vector3f>());
      } else if(Vector4f* vec4 = c.param(0).try_as<Vector4f>()) {
        c.return_value() = vec4->dist(c.param(1).get<Vector4f>());
      }
    });
    register_script_function("dot", [](ScriptContext& c) {
      if(Vector2f* vec2 = c.param(0).try_as<Vector2f>()) {
        c.return_value() = vec2->dot(c.param(1).get<Vector2f>());
      } else if(Vector3f* vec3 = c.param(0).try_as<Vector3f>()) {
        c.return_value() = vec3->dot(c.param(1).get<Vector3f>());
      } else if(Vector4f* vec4 = c.param(0).try_as<Vector4f>()) {
        c.return_value() = vec4->dot(c.param(1).get<Vector4f>());
      }
    });
    register_script_function("cross", [](ScriptContext& c) {
      c.return_value() = c.param(0).get<Vector3f>().cross(c.param(1).get<Vector3f>());
    });

    L_SCRIPT_NATIVE_ACCESS_METHOD(Vector2f, x);
    L_SCRIPT_NATIVE_ACCESS_METHOD(Vector2f, y);

    L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, x);
    L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, y);
    L_SCRIPT_NATIVE_ACCESS_METHOD(Vector3f, z);
  }
}