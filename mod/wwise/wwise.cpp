#include <L/src/audio/AudioEngine.h>
#include <L/src/component/NameComponent.h>
#include <L/src/component/Transform.h>
#include <L/src/dev/debug.h>
#include <L/src/engine/Engine.h>
#include <L/src/text/String.h>

#if !L_DBG
#define AK_OPTIMIZED
#endif

L_PUSH_NO_WARNINGS

#define UNICODE

#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>

#include <AkFilePackageLowLevelIOBlocking.h>

#if L_WINDOWS
#include <AK/SoundEngine/Platforms/Windows/AkTypes.h>
#endif

#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif

#include <AkDefaultIOHookBlocking.cpp>
#include <AkDefaultIOHookDeferred.cpp>
#include <AkDefaultLowLevelIODispatcher.cpp>
#include <AkFileLocationBase.cpp>
#include <AkFilePackage.cpp>
#include <AkFilePackageLUT.cpp>
#include <AkMultipleFileLocation.cpp>

#undef UNICODE

L_POP_NO_WARNINGS

using namespace L;

CAkFilePackageLowLevelIOBlocking low_level_io;

class WwiseAudioEngine : public AudioEngine {
protected:
  Table<Handle<Entity>, AkGameObjectID> _objects;
  AkGameObjectID _next_goid = 0;
  AkGameObjectID _listener_object_id = AK_INVALID_GAME_OBJECT;

public:
  virtual void init() override {
    AkMemSettings mem_settings;
    AK::MemoryMgr::GetDefaultSettings(mem_settings);
    if(AK::MemoryMgr::Init(&mem_settings) != AK_Success) {
      warning("wwise: Could not create the memory manager.");
      return;
    }

    AkStreamMgrSettings stream_settings;
    AK::StreamMgr::GetDefaultSettings(stream_settings);
    if(!AK::StreamMgr::Create(stream_settings)) {
      warning("wwise: Could not create the Streaming Manager");
      return;
    }

    AkDeviceSettings device_settings;
    AK::StreamMgr::GetDefaultDeviceSettings(device_settings);
    if(low_level_io.Init(device_settings) != AK_Success) {
      warning("wwise: Could not create the streaming device and Low-Level I/O system");
      return;
    }

    AkInitSettings init_settings;
    AkPlatformInitSettings platform_init_settings;
    AK::SoundEngine::GetDefaultInitSettings(init_settings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platform_init_settings);
    if(AK::SoundEngine::Init(&init_settings, &platform_init_settings) != AK_Success) {
      warning("wwise: Could not initialize the Sound Engine.");
      return;
    }

    AkMusicSettings music_init;
    AK::MusicEngine::GetDefaultInitSettings(music_init);
    if(AK::MusicEngine::Init(&music_init) != AK_Success) {
      warning("wwise: Could not initialize the Music Engine.");
      return;
    }

    AkSpatialAudioInitSettings spatial_audio_settings;
    if(AK::SpatialAudio::Init(spatial_audio_settings) != AK_Success) {
      warning("wwise: Could not initialize the Spatial Audio.");
      return;
    }

#ifndef AK_OPTIMIZED
    AkCommSettings comm_settings;
    AK::Comm::GetDefaultInitSettings(comm_settings);
    if(AK::Comm::Init(comm_settings) != AK_Success) {
      warning("wwise: Could not initialize communication.");
      return;
    }
#endif // AK_OPTIMIZED

    low_level_io.SetBasePath(AKTEXT("soundbanks"));
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    AkBankID bank_id;
    AK::SoundEngine::LoadBank(AKTEXT("Init"), bank_id);
    AK::SoundEngine::LoadBank(AKTEXT("Main"), bank_id);
  }
  void update() {
    { // Update objects
      Array<Handle<Entity>> stale_entities;

      // Update positions
      for(const auto& object : _objects) {
        if(const Entity* entity = object.key()) {
          if(Transform* transform = entity->get_component<Transform>()) {
            AK::SoundEngine::SetPosition(object.value(), get_ak_transform(*transform));
          }
        } else {
          stale_entities.push(object.key());
        }
      }

      // Remove stale entities
      for(Handle<Entity> stale_entity : stale_entities) {
        _objects.remove(stale_entity);
      }
    }

    AK::SoundEngine::RenderAudio();
  }

  virtual void set_listener(Handle<Entity> entity) override {
    _listener_object_id = get_goid_for_entity(entity);
    AK::SoundEngine::SetDefaultListeners(&_listener_object_id, 1);
  }
  virtual void post_event(const char* name, Handle<Entity> entity) override {
    AkGameObjectID goid = get_goid_for_entity(entity);
    if(goid == AK_INVALID_GAME_OBJECT) {
      // Default to listener game object for convenience
      // since PostEvent _requires_ a game object
      goid = _listener_object_id;
    }
    AK::SoundEngine::PostEvent(name, goid);
  }
  virtual void set_parameter(const char* name, float value, Handle<Entity> entity) override {
    AK::SoundEngine::SetRTPCValue(name, value, get_goid_for_entity(entity));
  }

  AkGameObjectID get_goid_for_entity(Handle<Entity> entity) {
    AkGameObjectID goid = AK_INVALID_GAME_OBJECT;
    if(AkGameObjectID* goid_ptr = _objects.find(entity)) {
      goid = *goid_ptr;
    } else if(entity.is_valid()) {
      goid = _next_goid;
      if(NameComponent* name_component = entity->get_component<NameComponent>()) {
        AK::SoundEngine::RegisterGameObj(_next_goid++, name_component->name());
      } else {
        AK::SoundEngine::RegisterGameObj(_next_goid++);
      }
    }
    return goid;
  }

  AkTransform get_ak_transform(const Transform& transform) {
    const Vector3f position = transform.position();
    const Vector3f forward = transform.forward();
    const Vector3f up = transform.up();
    AkTransform ak_transform;
    ak_transform.Set(
      AkVector{position.x(), position.y(), position.z()},
      AkVector{forward.x(), forward.y(), forward.z()},
      AkVector{up.x(), up.y(), up.z()});
    return ak_transform;
  }
};

void wwise_module_init() {
  static WwiseAudioEngine wwise_audio_engine;
  Engine::add_late_update([]() {
    wwise_audio_engine.update();
  });
}
