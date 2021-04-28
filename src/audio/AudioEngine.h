#pragma once

#include "../component/Entity.h"

namespace L {
  class AudioEngine {
  private:
    static AudioEngine* _instance;
  public:
    AudioEngine();
    virtual ~AudioEngine() {}

    virtual void init() = 0;

    virtual void set_listener(Handle<Entity> entity) = 0;
    virtual void post_event(const char* name, Handle<Entity> entity = nullptr) = 0;
    virtual void set_parameter(const char* name, float value, Handle<Entity> entity = nullptr) = 0;

    inline static AudioEngine* get() { return _instance; }
    static void init_instance();
    static void script_registration();
  };
}
