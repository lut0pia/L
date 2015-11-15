#ifndef DEF_L_Interface_wwise
#define DEF_L_Interface_wwise

#include <L/L.h>

#ifndef L_WINDOWS
# error Only for Windows
#endif

#ifndef L_Debug
# define AK_OPTIMIZED
#endif

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>        // Memory Manager
#include <AK/SoundEngine/Common/AkModule.h>           // Default memory and stream managers
#include <AK/SoundEngine/Common/IAkStreamMgr.h>       // Streaming Manager
#include <AK/SoundEngine/Common/AkSoundEngine.h>      // Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>      // Music Engine
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>  // AkStreamMgrModule
#include <AK/Tools/Common/AkPlatformFuncs.h>          // Thread defines
#include <../samples/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>          // Sample low-level I/O implementation
#ifndef AK_OPTIMIZED
# include <AK/Comm/AkCommunication.h>
#endif
#include <AK/Plugin/AllPluginsRegistrationHelpers.h>

namespace AK {
  inline void * AllocHook(size_t in_size) {
    return malloc(in_size);
  }
  inline void FreeHook(void * in_ptr) {
    free(in_ptr);
  }
  inline void * VirtualAllocHook(void * in_pMemAddress,size_t in_size,DWORD in_dwAllocationType,DWORD in_dwProtect) {
    return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
  }
  inline void VirtualFreeHook(void * in_pMemAddress,size_t in_size,DWORD in_dwFreeType) {
    VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
  }
}

namespace L {
  class Wwise {
    private:
      static bool _initialized;
      static CAkFilePackageLowLevelIOBlocking _lowLevelIO;
      static AkGameObjectID _nextGameObject, _maxGameObject;
    public:
      static void init(const wchar_t* basePath) {
        // Initialize memory manager
        AkMemSettings memSettings;
        memSettings.uMaxNumPools = 20;
        if(AK::MemoryMgr::Init(&memSettings) != AK_Success)
          throw Exception("Wwise: MemoryMgr init");
        // Initialize streaming manager
        AkStreamMgrSettings stmSettings;
        AK::StreamMgr::GetDefaultSettings(stmSettings);
        if(!AK::StreamMgr::Create(stmSettings))
          throw Exception("Wwise: StreamMgr init");
        // Create streaming device
        AkDeviceSettings deviceSettings;
        AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
        if(_lowLevelIO.Init(deviceSettings)!= AK_Success)
          throw Exception("Wwise: MusicEngine init");
        _lowLevelIO.SetBasePath(basePath);
        AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
        // Initialize sound engine
        AkInitSettings initSettings;
        AkPlatformInitSettings platformInitSettings;
        AK::SoundEngine::GetDefaultInitSettings(initSettings);
        AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
        if(AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
          throw Exception("Wwise: SoundEngine init");
        // Initialize music engine
        AkMusicSettings musicInit;
        AK::MusicEngine::GetDefaultInitSettings(musicInit);
        if(AK::MusicEngine::Init(&musicInit)  != AK_Success)
          throw Exception("Wwise: MusicEngine init");
#ifndef AK_OPTIMIZED
        // Initialize communication
        AkCommSettings commSettings;
        AK::Comm::GetDefaultInitSettings(commSettings);
        if(AK::Comm::Init(commSettings) != AK_Success)
          throw Exception("Wwise: Comm init");
#endif
        AK::SoundEngine::RegisterAllCodecPlugins();
        AK::SoundEngine::RegisterAllEffectPlugins();
        for(AkGameObjectID i(0); i<_maxGameObject; i++)
          AK::SoundEngine::RegisterGameObj(i);
      }
      static void term() {
#ifndef AK_OPTIMIZED
        // Terminate communication
        AK::Comm::Term();
#endif
        AK::MusicEngine::Term();
        AK::SoundEngine::Term();
      }
      static void update() {
        AK::SoundEngine::RenderAudio();
      }
      // Game objects
      static void registerObject(AkGameObjectID id) {
        AK::SoundEngine::RegisterGameObj(id);
      }
      // Events
      static AkGameObjectID postEvent(const String& name, const Vector3f& position = Vector3f(0,0,0)) {
        AkGameObjectID goid(_nextGameObject);
        _nextGameObject = (_nextGameObject+1)%_maxGameObject;
        AkSoundPosition soundPos = {{position.x(),position.y(),position.z()}};
        AK::SoundEngine::SetPosition(goid, soundPos);
        postEvent(name,goid);
        return goid;
      }
      static void postEvent(const String& name, AkGameObjectID goid) {
        AK::SoundEngine::PostEvent(name,goid);
      }
      // RTPC
      static void rtpc(const String& name, float value) {
        AK::SoundEngine::SetRTPCValue(name,value);
      }
      // Listeners
      static void listen(const Vector3f& pos, const Vector3f& front, const Vector3f& top) {
        AkListenerPosition lp = {{front.x(),front.y(),front.z()},{top.x(),top.y(),top.z()},{pos.x(),pos.y(),pos.z()}};
        AK::SoundEngine::SetListenerPosition(lp);
      }
      static void listen(const GL::Camera& cam) {
        listen(cam.position(),cam.forward(),cam.up());
      }
      // Sound banks
      static void loadBank(const wchar_t* name) {
        AkBankID bankID;
        if(AK::SoundEngine::LoadBank(name,AK_DEFAULT_POOL_ID,bankID) != AK_Success)
          throw Exception("Wwise: couldn't load bank");
      }
  };
}

#endif

