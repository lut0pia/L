#include <L/src/audio/AudioEngine.h>
#include <L/src/component/NameComponent.h>
#include <L/src/component/Transform.h>
#include <L/src/dev/debug.h>
#include <L/src/engine/Engine.h>
#include <L/src/text/String.h>

L_PUSH_NO_WARNINGS

#include <fmod.h>
#include <fmod_common.h>
#include <fmod_studio.h>

L_POP_NO_WARNINGS

using namespace L;

static class FMODAudioEngine* fmod_audio_engine = nullptr;

class FMODAudioEngine : public AudioEngine {
protected:
  FMOD_STUDIO_SYSTEM* _system = nullptr;

  struct Event {
    FMOD_STUDIO_EVENTDESCRIPTION* description;
    FMOD_STUDIO_EVENTINSTANCE* instance;
    Handle<Entity> entity;
  };

  Array<Event> _events;
  Handle<Entity> _listener;
  Table<Handle<Transform>, FMOD_3D_ATTRIBUTES> _transform_3d_attributes;

public:
  virtual void init() override {
    FMOD_Studio_System_Create(&_system, FMOD_VERSION);
    FMOD_Studio_System_Initialize(_system, 16, 0, 0, nullptr);
    FMOD_Studio_System_SetNumListeners(_system, 1);

    Engine::add_late_update([]() {
      fmod_audio_engine->update();
    });
    Engine::add_shutdown([]() {
      fmod_audio_engine->shutdown();
    });
  }
  void shutdown() {
    FMOD_Studio_System_Release(_system);
  }
  void update() {
    for(Event& event : _events) {
      if(Entity* entity = event.entity) {
        if(Transform* transform = entity->get_component<Transform>()) {
          FMOD_Studio_EventInstance_Set3DAttributes(event.instance, get_3d_attributes_for_transform(transform->handle()));
        }
      }
    }
    if(Entity* entity = _listener) {
      if(Transform* transform = entity->get_component<Transform>()) {
        FMOD_Studio_System_SetListenerAttributes(_system, 0, get_3d_attributes_for_transform(transform->handle()), nullptr);
      }
    }
    FMOD_Studio_System_Update(_system);
  }

  virtual void set_listener(Handle<Entity> entity) override {
    _listener = entity;
  }
  virtual void post_event(const Symbol& name, Handle<Entity> entity) override {
    Event event;
    event.entity = entity;
    FMOD_Studio_System_GetEvent(_system, name, &event.description);
    FMOD_Studio_EventDescription_CreateInstance(event.description, &event.instance);
    FMOD_Studio_EventInstance_Start(event.instance);
    _events.push(event);
  }
  virtual void set_parameter(const Symbol& name, float value, Handle<Entity>) override {
    FMOD_Studio_System_SetParameterByName(_system, name, value, false);
  }

  FMOD_3D_ATTRIBUTES* get_3d_attributes_for_transform(Handle<Transform> transform) {
    bool created = false;
    FMOD_3D_ATTRIBUTES* attributes = _transform_3d_attributes.find_or_create(transform, &created);
    const Vector3f position = transform->position();
    const Vector3f forward = transform->forward();
    const Vector3f up = transform->up();
    memcpy(&attributes->position, &position, sizeof(attributes->position));
    memset(&attributes->velocity, 0, sizeof(attributes->velocity));
    memcpy(&attributes->forward, &forward, sizeof(attributes->forward));
    memcpy(&attributes->up, &up, sizeof(attributes->up));
    return attributes;
  }
};

void fmod_module_init() {
  fmod_audio_engine = Memory::new_type<FMODAudioEngine>();
}
