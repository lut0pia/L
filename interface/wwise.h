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
//#include <../samples/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>          // Sample low-level I/O implementation
#ifndef AK_OPTIMIZED
# include <AK/Comm/AkCommunication.h>
#endif


namespace AK {
  void * AllocHook(size_t in_size) {
    return malloc(in_size);
  }
  void FreeHook(void * in_ptr) {
    free(in_ptr);
  }
  void * VirtualAllocHook(void * in_pMemAddress,size_t in_size,DWORD in_dwAllocationType,DWORD in_dwProtect) {
    return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
  }
  void VirtualFreeHook(void * in_pMemAddress,size_t in_size,DWORD in_dwFreeType) {
    VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
  }
}

namespace L {
  class Wwise {
    private:
      //CAkFilePackageLowLevelIOBlocking _lowLevelIO;
    public:
      Wwise() {
        // Initialize memory manager
        AkMemSettings memSettings;
        memSettings.uMaxNumPools = 20;
        if(AK::MemoryMgr::Init(&memSettings) != AK_Success)
          L_Error("Wwise: MemoryMgr init");
        // Initialize streaming manager
        AkStreamMgrSettings stmSettings;
        AK::StreamMgr::GetDefaultSettings(stmSettings);
        if(!AK::StreamMgr::Create(stmSettings))
          L_Error("Wwise: StreamMgr init");
        // Create streaming device
        /*
        AkDeviceSettings deviceSettings;
        AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
        if(_lowLevelIO.Init(deviceSettings)!= AK_Success)
          L_Error("Wwise: MusicEngine init");
        */
        // Initialize sound engine
        AkInitSettings initSettings;
        AkPlatformInitSettings platformInitSettings;
        AK::SoundEngine::GetDefaultInitSettings(initSettings);
        AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
        if(AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
          L_Error("Wwise: SoundEngine init");
        // Initialize music engine
        AkMusicSettings musicInit;
        AK::MusicEngine::GetDefaultInitSettings(musicInit);
        if(AK::MusicEngine::Init(&musicInit)  != AK_Success)
          L_Error("Wwise: MusicEngine init");
#ifndef AK_OPTIMIZED
        // Initialize communication
        AkCommSettings commSettings;
        AK::Comm::GetDefaultInitSettings(commSettings);
        if(AK::Comm::Init(commSettings) != AK_Success)
          L_Error("Wwise: Comm init");
#endif
      }
      ~Wwise() {
#ifndef AK_OPTIMIZED
        // Terminate communication
        AK::Comm::Term();
#endif
        AK::MusicEngine::Term();
        AK::SoundEngine::Term();
      }
      void update() {
        AK::SoundEngine::RenderAudio();
      }
      // Game objects
      void registerObject(AkGameObjectID id) {
        AK::SoundEngine::RegisterGameObj(id);
      }
      // Events
      void postEvent(const String& name) {
        AK::SoundEngine::PostEvent(name.c_str(),100);
      }
  };
}

#endif

