#include "AudioEngine.h"

#include "../script/script_binding.h"

using namespace L;

static Array<AudioEngine*> implementations;

AudioEngine* AudioEngine::_instance = nullptr;

AudioEngine::AudioEngine() {
  implementations.push(this);
}

void AudioEngine::init_instance() {
  if(implementations.size() == 0) {
    log("No audio engine implementation available");
    return;
  }
  
  script_registration();

  _instance = implementations[0];
  _instance->init();
}

static Handle<Entity> get_optional_entity(ScriptContext& c, uintptr_t i) {
  Handle<Entity> entity;
  if(c.param_count() > i) {
    if(Handle<Entity>* entity_ptr = c.param(i).try_as<Handle<Entity>>()) {
      entity = *entity_ptr;
    }
  }
  return entity;
}

void AudioEngine::script_registration() {
  register_script_function("set_audio_listener", [](ScriptContext& c) {
    if(_instance && c.param_count() > 0) {
      _instance->set_listener(c.param(0).get<Handle<Entity>>());
    }
  });

  register_script_function("post_audio_event", [](ScriptContext& c) {
    if(_instance && c.param_count() > 0) {
      _instance->post_event(c.param(0).get<String>(), get_optional_entity(c, 1));
    }
  });

  register_script_function("set_audio_parameter", [](ScriptContext& c) {
    if(_instance && c.param_count() > 0) {
      _instance->set_parameter(c.param(0).get<String>(), c.param(1).get<float>(), get_optional_entity(c, 2));
    }
  });
}