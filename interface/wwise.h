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
      static bool initialized;
      static CAkFilePackageLowLevelIOBlocking _lowLevelIO;
    public:
      static void init(const wchar_t* basePath) {
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
        AkDeviceSettings deviceSettings;
        AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
        if(_lowLevelIO.Init(deviceSettings)!= AK_Success)
          L_Error("Wwise: MusicEngine init");
        _lowLevelIO.SetBasePath(basePath);
        AK::StreamMgr::SetCurrentLanguage( AKTEXT("English(US)") );
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
      static void postEvent(const String& name) {
        AK::SoundEngine::PostEvent(name.c_str(),100);
      }
      // Sound banks
      static void loadBank(const wchar_t* name) {
        AkBankID bankID;
        if(AK::SoundEngine::LoadBank(name,AK_DEFAULT_POOL_ID,bankID) != AK_Success)
          L_Error("Wwise: couldn't load bank");
      }
  };
}
bool L::Wwise::initialized(false);
CAkFilePackageLowLevelIOBlocking L::Wwise::_lowLevelIO;

// Destroy file package and free memory / destroy pool.
void CAkFilePackage::Destroy() {
  // Cache memory pointer and pool ID because memory pool is destroyed _after_ deleting this.
  AkMemPoolId	poolID	= m_poolID;
  void * pToRelease	= m_pToRelease;
  bool bIsInternalPool = m_bIsInternalPool;
  // Call destructor.
  this->~CAkFilePackage();
  // Free memory.
  ClearMemory(poolID, pToRelease, bIsInternalPool);
}

void CAkFilePackage::ClearMemory(
  AkMemPoolId in_poolID,			// Pool to destroy.
  void *		in_pMemToRelease,	// Memory block to free before destroying pool.
  bool		in_bIsInternalPool	// Pool was created internally (and needs to be destroyed).
) {
  if(in_poolID != AK_INVALID_POOL_ID) {
    if(in_pMemToRelease) {
      if(in_bIsInternalPool) {
        AK::MemoryMgr::ReleaseBlock(in_poolID, in_pMemToRelease);
        // Destroy pool
        AKVERIFY(AK::MemoryMgr::DestroyPool(in_poolID) == AK_Success);
      } else {
        if(AK::MemoryMgr::GetPoolAttributes(in_poolID) & AkBlockMgmtMask)
          AK::MemoryMgr::ReleaseBlock(in_poolID, in_pMemToRelease);
        else
          AkFree(in_poolID, in_pMemToRelease);
      }
    } else
      AKASSERT(!in_bIsInternalPool);	// Internal pools allocation is guaranteed.
  }
}

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>	// For string hash.
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/Tools/Common/AkFNVHash.h>

#ifdef _DEBUG
template<bool> struct AkCompileTimeAssert;
template<> struct AkCompileTimeAssert<true> { };
#define AK_STATIC_ASSERT(e) (AkCompileTimeAssert<(e) != 0>())
#else
#define AK_STATIC_ASSERT(e)
#endif

#define AK_MAX_EXTERNAL_NAME_SIZE		260

CAkFilePackageLUT::CAkFilePackageLUT()
  :m_curLangID(AK_INVALID_LANGUAGE_ID)
  ,m_pLangMap(NULL)
  ,m_pSoundBanks(NULL)
  ,m_pStmFiles(NULL)
  ,m_pExternals(NULL) {
  AK_STATIC_ASSERT(sizeof(AkFileEntry<AkFileID>) == 20);
  AK_STATIC_ASSERT(sizeof(AkFileEntry<AkUInt64>) == 24);
}

CAkFilePackageLUT::~CAkFilePackageLUT() {
}

// Create a new LUT from a packaged file header.
// The LUT sets pointers to appropriate location inside header data (in_pData).
AKRESULT CAkFilePackageLUT::Setup(
  AkUInt8 *			in_pData,			// Header data.
  AkUInt32			in_uHeaderSize		// Size of file package header.
) {
  struct FileHeaderFormat {
    char				headerDefinition[AKPK_HEADER_CHUNK_DEF_SIZE];
    AkUInt32			uVersion;
    AkUInt32			uLanguageMapSize;
    AkUInt32			uSoundBanksLUTSize;
    AkUInt32			uStmFilesLUTSize;
    AkUInt32			uExternalsLUTSize;
  };
  FileHeaderFormat * pHeader = (FileHeaderFormat*)in_pData;
  // Check header size,
  if(in_uHeaderSize < sizeof(FileHeaderFormat)
      + pHeader->uLanguageMapSize
      + pHeader->uSoundBanksLUTSize
      + pHeader->uStmFilesLUTSize
      + pHeader->uExternalsLUTSize) {
    return AK_Fail;
  }
  // Check version.
  if(pHeader->uVersion < AKPK_CURRENT_VERSION)
    return AK_Fail;
  // Get address of maps and LUTs.
  in_pData += sizeof(FileHeaderFormat);
  m_pLangMap		= (StringMap*)in_pData;
  in_pData += pHeader->uLanguageMapSize;
  m_pSoundBanks	= (FileLUT<AkFileID>*)in_pData;
  in_pData += pHeader->uSoundBanksLUTSize;
  m_pStmFiles		= (FileLUT<AkFileID>*)in_pData;
  in_pData += pHeader->uStmFilesLUTSize;
  m_pExternals	= (FileLUT<AkUInt64>*)in_pData;
  return AK_Success;
}

// Find a file entry by ID.
const CAkFilePackageLUT::AkFileEntry<AkFileID> * CAkFilePackageLUT::LookupFile(
  AkFileID			in_uID,			// File ID.
  AkFileSystemFlags * in_pFlags		// Special flags. Do not pass NULL.
) {
  AKASSERT(in_pFlags && in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC);
  if(in_pFlags->uCodecID == AKCODECID_BANK
      && m_pSoundBanks
      && m_pSoundBanks->HasFiles()) {
    return LookupFile<AkFileID>(in_uID, m_pSoundBanks, in_pFlags->bIsLanguageSpecific);
  } else if(m_pStmFiles && m_pStmFiles->HasFiles()) {
    // We assume that the file is a streamed audio file.
    return LookupFile<AkFileID>(in_uID, m_pStmFiles, in_pFlags->bIsLanguageSpecific);
  }
  // No table loaded.
  return NULL;
}

// Find a file entry by ID.
const CAkFilePackageLUT::AkFileEntry<AkUInt64> * CAkFilePackageLUT::LookupFile(
  AkUInt64			in_uID,			// File ID.
  AkFileSystemFlags * in_pFlags		// Special flags. Do not pass NULL.
) {
  AKASSERT(in_pFlags);
  if(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL
      && m_pExternals
      && m_pExternals->HasFiles()) {
    return LookupFile<AkUInt64>(in_uID, m_pExternals, in_pFlags->bIsLanguageSpecific);
  }
  // No table loaded.
  return NULL;
}

// Set current language.
// Returns AK_InvalidLanguage if a package is loaded but the language string cannot be found.
// Returns AK_Success otherwise.
AKRESULT CAkFilePackageLUT::SetCurLanguage(
  const AkOSChar*			in_pszLanguage	// Language string.
) {
  m_curLangID = AK_INVALID_LANGUAGE_ID;
  if(m_pLangMap && in_pszLanguage) {
    AkUInt16 uLangID = (AkUInt16)m_pLangMap->GetID(in_pszLanguage);
    if(uLangID == AK_INVALID_UNIQUE_ID
        && m_pLangMap->GetNumStrings() > 1) {	// Do not return AK_InvalidLanguage if package contains only SFX data.
      return AK_InvalidLanguage;
    }
    m_curLangID = uLangID;
  }
  return AK_Success;
}

void CAkFilePackageLUT::RemoveFileExtension(AkOSChar* in_pstring) {
  int i = (int)AKPLATFORM::OsStrLen(in_pstring) - 1;
  while(i >= 0) {
    if(in_pstring[i] == AKTEXT('.')) {
      in_pstring[i] = AKTEXT('\0');
      return;
    }
    i--;
  }
}

// Find a soundbank ID by its name.
// Returns AK_INVALID_FILE_ID if no soundbank LUT is loaded.
AkFileID CAkFilePackageLUT::GetSoundBankID(
  const AkOSChar*			in_pszBankName	// Soundbank name.
) {
  // Remove the file extension if it was used.
  AkUInt32 stringSize = (AkUInt32)AKPLATFORM::OsStrLen(in_pszBankName) + 1;
  AkOSChar* pStringWithoutExtension = (AkOSChar *)AkAlloca((stringSize) * sizeof(AkOSChar));
  AKPLATFORM::SafeStrCpy(pStringWithoutExtension, in_pszBankName, stringSize);
  RemoveFileExtension(pStringWithoutExtension);
  // Hash
  return AK::SoundEngine::GetIDFromString(pStringWithoutExtension);
}

AkUInt64 CAkFilePackageLUT::GetExternalID(
  const AkOSChar*			in_pszExternalName		// External Source name.
) {
  char* szString;
  CONVERT_OSCHAR_TO_CHAR(in_pszExternalName, szString);
  size_t stringSize = strlen(szString);
  // 1- Make lower case.
  _MakeLowerA(szString, stringSize);
  AK::FNVHash64 MainHash;
  return MainHash.Compute((const unsigned char *) szString, (unsigned int)stringSize);
}

void CAkFilePackageLUT::_MakeLowerA(char* in_pString, size_t in_strlen) {
  for(size_t i = 0; i < in_strlen; ++i) {
    if(in_pString[i] >= 'A' && in_pString[i] <= 'Z') {
      in_pString[i] += 0x20;
    }
  }
}

void CAkFilePackageLUT::_MakeLower(AkOSChar* in_pString) {
  size_t uStrlen = AKPLATFORM::OsStrLen(in_pString);
  const AkOSChar CaseDiff = AKTEXT('a') - AKTEXT('A');
  for(size_t i = 0; i < uStrlen; ++i) {
    if(in_pString[i] >= AKTEXT('A') && in_pString[i] <= AKTEXT('Z')) {
      in_pString[i] += CaseDiff;
    }
  }
}

AkUInt32 CAkFilePackageLUT::StringMap::GetID(const AkOSChar* in_pszString) {
  // Make string lower case.
  size_t uStrLen = AKPLATFORM::OsStrLen(in_pszString)+1;
  AkOSChar * pszLowerCaseString = (AkOSChar*)AkAlloca(uStrLen*sizeof(AkOSChar));
  AKASSERT(pszLowerCaseString);
  AKPLATFORM::SafeStrCpy(pszLowerCaseString, in_pszString, uStrLen);
  _MakeLower(pszLowerCaseString);
  // 'this' is m_uNumStrings. +1 points to the beginning of the StringEntry array.
  StringEntry * pTable = (StringEntry*)((AkUInt32*)this + 1);
  // Binary search: strings are sorted (case sensitive).
  AkInt32 uTop = 0, uBottom = m_uNumStrings-1;
  do {
    AkInt32 uThis = (uBottom - uTop) / 2 + uTop;
    AkOSChar * pString = (AkOSChar*)((AkUInt8*)this + pTable[ uThis ].uOffset);
    int iCmp = AKPLATFORM::OsStrCmp(pString, pszLowerCaseString);
    if(0 == iCmp)
      return pTable[uThis].uID;
    else if(iCmp > 0)	//in_pTable[ uThis ].pString > pszLowerCaseString
      uBottom = uThis - 1;
    else					//in_pTable[ uThis ].pString < pszLowerCaseString
      uTop = uThis + 1;
  } while(uTop <= uBottom);
  // ID not found.
  return AK_INVALID_UNIQUE_ID;
}


#define WIN32_BLOCKING_DEVICE_NAME		(AKTEXT("Win32 Blocking"))	// Default blocking device name.

CAkDefaultIOHookBlocking::CAkDefaultIOHookBlocking()
  : m_deviceID(AK_INVALID_DEVICE_ID)
  , m_bAsyncOpen(false) {
}

CAkDefaultIOHookBlocking::~CAkDefaultIOHookBlocking() {
}

// Initialization/termination. Init() registers this object as the one and
// only File Location Resolver if none were registered before. Then
// it creates a streaming device with scheduler type AK_SCHEDULER_BLOCKING.
AKRESULT CAkDefaultIOHookBlocking::Init(
  const AkDeviceSettings &	in_deviceSettings,		// Device settings.
  bool						in_bAsyncOpen/*=false*/	// If true, files are opened asynchronously when possible.
) {
  if(in_deviceSettings.uSchedulerTypeFlags != AK_SCHEDULER_BLOCKING) {
    AKASSERT(!"CAkDefaultIOHookBlocking I/O hook only works with AK_SCHEDULER_BLOCKING devices");
    return AK_Fail;
  }
  m_bAsyncOpen = in_bAsyncOpen;
  // If the Stream Manager's File Location Resolver was not set yet, set this object as the
  // File Location Resolver (this I/O hook is also able to resolve file location).
  if(!AK::StreamMgr::GetFileLocationResolver())
    AK::StreamMgr::SetFileLocationResolver(this);
  // Create a device in the Stream Manager, specifying this as the hook.
  m_deviceID = AK::StreamMgr::CreateDevice(in_deviceSettings, this);
  if(m_deviceID != AK_INVALID_DEVICE_ID)
    return AK_Success;
  return AK_Fail;
}

void CAkDefaultIOHookBlocking::Term() {
  if(AK::StreamMgr::GetFileLocationResolver() == this)
    AK::StreamMgr::SetFileLocationResolver(NULL);
  AK::StreamMgr::DestroyDevice(m_deviceID);
}

//
// IAkFileLocationAware interface.
//-----------------------------------------------------------------------------

// Returns a file descriptor for a given file name (string).
AKRESULT CAkDefaultIOHookBlocking::Open(
  const AkOSChar* in_pszFileName,     // File name.
  AkOpenMode      in_eOpenMode,       // Open mode.
  AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
  bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
  AkFileDesc &    out_fileDesc        // Returned file descriptor.
) {
  // We normally consider that calls to ::CreateFile() on a hard drive are fast enough to execute in the
  // client thread. If you want files to be opened asynchronously when it is possible, this device should
  // be initialized with the flag in_bAsyncOpen set to true.
  if(io_bSyncOpen || !m_bAsyncOpen) {
    io_bSyncOpen = true;
    // Get the full file path, using path concatenation logic.
    AkOSChar szFullFilePath[AK_MAX_PATH];
    if(GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath) == AK_Success) {
      // Open the file without FILE_FLAG_OVERLAPPED and FILE_FLAG_NO_BUFFERING flags.
      AKRESULT eResult = CAkFileHelpers::OpenFile(
                           szFullFilePath,
                           in_eOpenMode,
                           false,
                           false,
                           out_fileDesc.hFile);
      if(eResult == AK_Success) {
#ifdef AK_USE_METRO_API
        FILE_STANDARD_INFO info;
        ::GetFileInformationByHandleEx(out_fileDesc.hFile, FileStandardInfo, &info, sizeof(info));
        out_fileDesc.iFileSize = info.EndOfFile.QuadPart;
#else
        ULARGE_INTEGER Temp;
        Temp.LowPart = ::GetFileSize(out_fileDesc.hFile,(LPDWORD)&Temp.HighPart);
        out_fileDesc.iFileSize			= Temp.QuadPart;
#endif
        out_fileDesc.uSector			= 0;
        out_fileDesc.deviceID			= m_deviceID;
        out_fileDesc.pCustomParam		= NULL;
        out_fileDesc.uCustomParamSize	= 0;
      }
      return eResult;
    }
    return AK_Fail;
  } else {
    // The client allows us to perform asynchronous opening.
    // We only need to specify the deviceID, and leave the boolean to false.
    out_fileDesc.iFileSize			= 0;
    out_fileDesc.uSector			= 0;
    out_fileDesc.deviceID			= m_deviceID;
    out_fileDesc.pCustomParam		= NULL;
    out_fileDesc.uCustomParamSize	= 0;
    return AK_Success;
  }
}

// Returns a file descriptor for a given file ID.
AKRESULT CAkDefaultIOHookBlocking::Open(
  AkFileID        in_fileID,          // File ID.
  AkOpenMode      in_eOpenMode,       // Open mode.
  AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
  bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
  AkFileDesc &    out_fileDesc        // Returned file descriptor.
) {
  // We normally consider that calls to ::CreateFile() on a hard drive are fast enough to execute in the
  // client thread. If you want files to be opened asynchronously when it is possible, this device should
  // be initialized with the flag in_bAsyncOpen set to true.
  if(io_bSyncOpen || !m_bAsyncOpen) {
    io_bSyncOpen = true;
    // Get the full file path, using path concatenation logic.
    AkOSChar szFullFilePath[AK_MAX_PATH];
    if(GetFullFilePath(in_fileID, in_pFlags, in_eOpenMode, szFullFilePath) == AK_Success) {
      // Open the file without FILE_FLAG_OVERLAPPED and FILE_FLAG_NO_BUFFERING flags.
      AKRESULT eResult = CAkFileHelpers::OpenFile(
                           szFullFilePath,
                           in_eOpenMode,
                           false,
                           false,
                           out_fileDesc.hFile);
      if(eResult == AK_Success) {
#ifdef AK_USE_METRO_API
        FILE_STANDARD_INFO info;
        ::GetFileInformationByHandleEx(out_fileDesc.hFile, FileStandardInfo, &info, sizeof(info));
        out_fileDesc.iFileSize = info.EndOfFile.QuadPart;
#else
        ULARGE_INTEGER Temp;
        Temp.LowPart = ::GetFileSize(out_fileDesc.hFile,(LPDWORD)&Temp.HighPart);
        out_fileDesc.iFileSize			= Temp.QuadPart;
#endif
        out_fileDesc.uSector			= 0;
        out_fileDesc.deviceID			= m_deviceID;
        out_fileDesc.pCustomParam		= NULL;
        out_fileDesc.uCustomParamSize	= 0;
      }
      return eResult;
    }
    return AK_Fail;
  } else {
    // The client allows us to perform asynchronous opening.
    // We only need to specify the deviceID, and leave the boolean to false.
    out_fileDesc.iFileSize			= 0;
    out_fileDesc.uSector			= 0;
    out_fileDesc.deviceID			= m_deviceID;
    out_fileDesc.pCustomParam		= NULL;
    out_fileDesc.uCustomParamSize	= 0;
    return AK_Success;
  }
}

//
// IAkIOHookBlocking implementation.
//-----------------------------------------------------------------------------

// Reads data from a file (synchronous).
AKRESULT CAkDefaultIOHookBlocking::Read(
  AkFileDesc &			in_fileDesc,        // File descriptor.
  const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
  void *					out_pBuffer,        // Buffer to be filled with data.
  AkIOTransferInfo &		io_transferInfo		// Synchronous data transfer info.
) {
  AKASSERT(out_pBuffer &&
           in_fileDesc.hFile != INVALID_HANDLE_VALUE);
  OVERLAPPED overlapped;
  overlapped.Offset = (DWORD)(io_transferInfo.uFilePosition & 0xFFFFFFFF);
  overlapped.OffsetHigh = (DWORD)((io_transferInfo.uFilePosition >> 32) & 0xFFFFFFFF);
  overlapped.hEvent = NULL;
  AkUInt32 uSizeTransferred;
  if(::ReadFile(
        in_fileDesc.hFile,
        out_pBuffer,
        io_transferInfo.uRequestedSize,
        &uSizeTransferred,
        &overlapped)) {
    AKASSERT(uSizeTransferred == io_transferInfo.uRequestedSize);
    return AK_Success;
  }
  return AK_Fail;
}

// Writes data to a file (synchronous).
AKRESULT CAkDefaultIOHookBlocking::Write(
  AkFileDesc &			in_fileDesc,        // File descriptor.
  const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
  void *					in_pData,           // Data to be written.
  AkIOTransferInfo &		io_transferInfo		// Synchronous data transfer info.
) {
  AKASSERT(in_pData &&
           in_fileDesc.hFile != INVALID_HANDLE_VALUE);
  OVERLAPPED overlapped;
  overlapped.Offset = (DWORD)(io_transferInfo.uFilePosition & 0xFFFFFFFF);
  overlapped.OffsetHigh = (DWORD)((io_transferInfo.uFilePosition >> 32) & 0xFFFFFFFF);
  overlapped.hEvent = NULL;
  AkUInt32 uSizeTransferred;
  if(::WriteFile(
        in_fileDesc.hFile,
        in_pData,
        io_transferInfo.uRequestedSize,
        &uSizeTransferred,
        &overlapped)) {
    AKASSERT(uSizeTransferred == io_transferInfo.uRequestedSize);
    return AK_Success;
  }
  return AK_Fail;
}

// Cleans up a file.
AKRESULT CAkDefaultIOHookBlocking::Close(
  AkFileDesc & in_fileDesc      // File descriptor.
) {
  return CAkFileHelpers::CloseFile(in_fileDesc.hFile);
}

// Returns the block size for the file or its storage device.
AkUInt32 CAkDefaultIOHookBlocking::GetBlockSize(
  AkFileDesc &  /*in_fileDesc*/     // File descriptor.
) {
  // No constraint on block size (file seeking).
  return 1;
}


// Returns a description for the streaming device above this low-level hook.
void CAkDefaultIOHookBlocking::GetDeviceDesc(
  AkDeviceDesc &
#ifndef AK_OPTIMIZED
  out_deviceDesc      // Description of associated low-level I/O device.
#endif
) {
#ifndef AK_OPTIMIZED
  AKASSERT(m_deviceID != AK_INVALID_DEVICE_ID || !"Low-Level device was not initialized");
  out_deviceDesc.deviceID       = m_deviceID;
  out_deviceDesc.bCanRead       = true;
  out_deviceDesc.bCanWrite      = true;
  AKPLATFORM::SafeStrCpy(out_deviceDesc.szDeviceName, WIN32_BLOCKING_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH);
  out_deviceDesc.uStringSize   = (AkUInt32)wcslen(out_deviceDesc.szDeviceName) + 1;
#endif
}

// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
AkUInt32 CAkDefaultIOHookBlocking::GetDeviceData() {
  return (m_bAsyncOpen) ? 1 : 0;
}

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#ifdef AK_WIN
#include <AK/Plugin/AkMP3SourceFactory.h> // For MP3 Codec ID.
#endif
#include <AK/Tools/Common/AkPlatformFuncs.h>
#ifdef AK_SUPPORT_WCHAR
#include <wchar.h>
#endif //AK_SUPPORT_WCHAR
#include <stdio.h>
#include <AK/Tools/Common/AkAssert.h>

#define MAX_NUMBER_STRING_SIZE      (10)    // 4G
#define ID_TO_STRING_FORMAT_BANK    AKTEXT("%u.bnk")
#define ID_TO_STRING_FORMAT_WEM     AKTEXT("%u.wem")
#define MAX_EXTENSION_SIZE          (4)     // .xxx
#define MAX_FILETITLE_SIZE          (MAX_NUMBER_STRING_SIZE+MAX_EXTENSION_SIZE+1)   // null-terminated

template< class TYPE > inline
const TYPE& AkTemplMax(const TYPE& in_left, const TYPE& in_right) {
  return (in_left < in_right) ? in_right : in_left;
}


CAkFileLocationBase::CAkFileLocationBase() {
  m_szBasePath[0] = '\0';
  m_szBankPath[0] = '\0';
  m_szAudioSrcPath[0] = '\0';
}

CAkFileLocationBase::~CAkFileLocationBase() {
}

// String overload.
// Returns AK_Success if input flags are supported and the resulting path is not too long.
// Returns AK_Fail otherwise.
AKRESULT CAkFileLocationBase::GetFullFilePath(
  const AkOSChar*		in_pszFileName,		// File name.
  AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
  AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
  AkOSChar*			out_pszFullFilePath // Full file path.
) {
  if(!in_pszFileName) {
    AKASSERT(!"Invalid file name");
    return AK_InvalidParameter;
  }
  // Prepend string path (basic file system logic).
  // Compute file name with file system paths.
  size_t uiPathSize = AKPLATFORM::OsStrLen(in_pszFileName);
  if(uiPathSize >= AK_MAX_PATH) {
    AKASSERT(!"Input string too large");
    return AK_InvalidParameter;
  }
#ifdef AK_WIN
  // MP3 files using the MP3 sample code, usually being provided by the gamer will
  // not be located in the game path, for these sounds, we are using the Full path
  // to access them.
  if(in_pFlags != NULL &&
      in_pFlags->uCodecID == AKSOURCEID_MP3 &&
      in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC) {
    out_pszFullFilePath[0] = 0;
  } else
#endif
  {
    AKPLATFORM::SafeStrCpy(out_pszFullFilePath, m_szBasePath, AK_MAX_PATH);
  }
  if(in_pFlags
      && in_eOpenMode == AK_OpenModeRead) {
    // Add bank path if file is an AK sound bank.
    if(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC &&
        in_pFlags->uCodecID == AKCODECID_BANK) {
      uiPathSize += AKPLATFORM::OsStrLen(m_szBankPath);
      if(uiPathSize >= AK_MAX_PATH) {
        AKASSERT(!"Path is too large");
        return AK_Fail;
      }
      AKPLATFORM::SafeStrCat(out_pszFullFilePath, m_szBankPath, AK_MAX_PATH);
    }
    // Note: Standard streaming files do not use this overload. On the other hand, streaming external
    // sources use it if you use AkExternalSourceInfo::szFile instead of AkExternalSourceInfo::idFile.
    // Externally supplied source (see External Sources in SDK doc)
    // In this sample, we will assume that the external source file name in_pszFileName
    // must be used as is (e.g. "myExternalSourceFile.wem").  If you use the External Source feature
    // you should modify this section to handle your FileIDs properly.
    /*if (in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL)
    {

    }*/
    // Add language directory name if needed.
    if(in_pFlags->bIsLanguageSpecific) {
      size_t uLanguageStrLen = AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage());
      if(uLanguageStrLen > 0) {
        uiPathSize += (uLanguageStrLen + 1);
        if(uiPathSize >= AK_MAX_PATH) {
          AKASSERT(!"Path is too large");
          return AK_Fail;
        }
        AKPLATFORM::SafeStrCat(out_pszFullFilePath, AK::StreamMgr::GetCurrentLanguage(), AK_MAX_PATH);
        AKPLATFORM::SafeStrCat(out_pszFullFilePath, AK_PATH_SEPARATOR, AK_MAX_PATH);
      }
    }
  }
  // Append file title.
  uiPathSize += AKPLATFORM::OsStrLen(out_pszFullFilePath);
  if(uiPathSize >= AK_MAX_PATH) {
    AKASSERT(!"File name string too large");
    return AK_Fail;
  }
  AKPLATFORM::SafeStrCat(out_pszFullFilePath, in_pszFileName, AK_MAX_PATH);
  return AK_Success;
}

// ID overload.
// The name of the file will be formatted as ID.ext. This is meant to be used with options
// "Use SoundBank Names" unchecked, and/or "Copy Streamed Files" in the SoundBank Settings.
// For more details, refer to the SoundBank Settings in Wwise Help, and to section "Identifying Banks" inside
// "Sound Engine Integration Walkthrough > Integrate Wwise Elements into Your Game > Integrating Banks >
// Integration Details - Banks > General Information" of the SDK documentation.
// Returns AK_Success if input flags are supported and the resulting path is not too long.
// Returns AK_Fail otherwise.
AKRESULT CAkFileLocationBase::GetFullFilePath(
  AkFileID			in_fileID,			// File ID.
  AkFileSystemFlags *	in_pFlags,			// Special flags.
  AkOpenMode			/* in_eOpenMode*/,	// File open mode (read, write, ...).
  AkOSChar *			out_pszFullFilePath	// Full file path.
) {
  // If the file descriptor could not be found, or if the script-based FS does not exist,
  // map file ID to file descriptor (string based) for Audiokinetic IDs.
  if(!in_pFlags ||
      !(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC || in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL)) {
    AKASSERT(!"Unhandled file type");
    return AK_Fail;
  }
  // Compute file name with file system paths.
  size_t uiPathSize = AKPLATFORM::OsStrLen(m_szBasePath);
  // Copy base path.
  AKPLATFORM::SafeStrCpy(out_pszFullFilePath, m_szBasePath, AK_MAX_PATH);
  // Concatenate path for AK banks or streamed audio files (everything except banks).
  if(in_pFlags->uCodecID == AKCODECID_BANK) {
    uiPathSize += AKPLATFORM::OsStrLen(m_szBankPath);
    if(uiPathSize >= AK_MAX_PATH) {
      AKASSERT(!"Path is too large");
      return AK_Fail;
    }
    AKPLATFORM::SafeStrCat(out_pszFullFilePath, m_szBankPath, AK_MAX_PATH);
  } else {
    uiPathSize += AKPLATFORM::OsStrLen(m_szAudioSrcPath);
    if(uiPathSize >= AK_MAX_PATH) {
      AKASSERT(!"Path is too large");
      return AK_Fail;
    }
    AKPLATFORM::SafeStrCat(out_pszFullFilePath, m_szAudioSrcPath, AK_MAX_PATH);
  }
  // Externally supplied source (see External Sources in SDK doc)
  // In this sample, we will assume that the file to load when receiving an external FileID is
  // simply the FileID.wem (e.g. "12345.wem").  If you use the External Source feature
  // you should modify this section to handle your FileIDs properly.
  /*if (in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL)
  {

  }*/
  // Add language directory name if needed.
  if(in_pFlags->bIsLanguageSpecific) {
    size_t uLanguageStrLen = AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage());
    if(uLanguageStrLen > 0) {
      uiPathSize += (uLanguageStrLen + 1);
      if(uiPathSize >= AK_MAX_PATH) {
        AKASSERT(!"Path is too large");
        return AK_Fail;
      }
      AKPLATFORM::SafeStrCat(out_pszFullFilePath, AK::StreamMgr::GetCurrentLanguage(), AK_MAX_PATH);
      AKPLATFORM::SafeStrCat(out_pszFullFilePath, AK_PATH_SEPARATOR, AK_MAX_PATH);
    }
  }
  // Append file title.
  if((uiPathSize + MAX_FILETITLE_SIZE) <= AK_MAX_PATH) {
    AkOSChar * pszTitle = out_pszFullFilePath + uiPathSize;
    if(in_pFlags->uCodecID == AKCODECID_BANK)
      AK_OSPRINTF(pszTitle, MAX_FILETITLE_SIZE, ID_TO_STRING_FORMAT_BANK, (unsigned int)in_fileID);
    else
      AK_OSPRINTF(pszTitle, MAX_FILETITLE_SIZE, ID_TO_STRING_FORMAT_WEM, (unsigned int)in_fileID);
  } else {
    AKASSERT(!"String buffer too small");
    return AK_Fail;
  }
  return AK_Success;
}

AKRESULT CAkFileLocationBase::SetBasePath(
  const AkOSChar*   in_pszBasePath
) {
  if(AKPLATFORM::OsStrLen(in_pszBasePath) + AkTemplMax(AKPLATFORM::OsStrLen(m_szBankPath), AKPLATFORM::OsStrLen(m_szAudioSrcPath)) + AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage()) + 1 >= AK_MAX_PATH) {
    return AK_InvalidParameter;
  }
  //Copy the base path EVEN if the directory does not exist.
  AKPLATFORM::SafeStrCpy(m_szBasePath, in_pszBasePath, AK_MAX_PATH);
  AKRESULT eDirectoryResult = CAkFileHelpers::CheckDirectoryExists(in_pszBasePath);
  if(eDirectoryResult == AK_Fail) { // AK_NotImplemented could be returned and should be ignored.
    return AK_PathNotFound;
  }
  return AK_Success;
}

AKRESULT CAkFileLocationBase::SetBankPath(
  const AkOSChar*   in_pszBankPath
) {
  if(AKPLATFORM::OsStrLen(m_szBasePath) + AkTemplMax(AKPLATFORM::OsStrLen(in_pszBankPath), AKPLATFORM::OsStrLen(m_szAudioSrcPath)) + AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage()) + 1 >= AK_MAX_PATH) {
    return AK_InvalidParameter;
  }
  AKPLATFORM::SafeStrCpy(m_szBankPath, in_pszBankPath, AK_MAX_PATH);
  return AK_Success;
}

AKRESULT CAkFileLocationBase::SetAudioSrcPath(
  const AkOSChar*   in_pszAudioSrcPath
) {
  if(AKPLATFORM::OsStrLen(m_szBasePath) + AkTemplMax(AKPLATFORM::OsStrLen(m_szBankPath), AKPLATFORM::OsStrLen(in_pszAudioSrcPath)) + AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage()) + 1 >= AK_MAX_PATH) {
    return AK_InvalidParameter;
  }
  AKPLATFORM::SafeStrCpy(m_szAudioSrcPath, in_pszAudioSrcPath, AK_MAX_PATH);
  return AK_Success;
}


#endif

