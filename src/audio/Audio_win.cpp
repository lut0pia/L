#include "Audio.h"

#include <Mmdeviceapi.h>
#include <mmreg.h>
#include <mfapi.h>
#include <AudioClient.h>

#include "../dev/debug.h"
#include "../stream/CFileStream.h"
#include "../system/Memory.h"

using namespace L;
using namespace Audio;

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { debugbreak(); }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const GUID PcmSubformatGuid = {STATIC_KSDATAFORMAT_SUBTYPE_PCM};

HRESULT hr;
IMMDeviceEnumerator *pEnumerator = NULL;
IMMDevice *pDevice = NULL;
IAudioClient *pAudioClient = NULL;
IAudioRenderClient *pRenderClient = NULL;
uint32_t internal_freq;

static void init_wave_format(WAVEFORMATEXTENSIBLE * format_ext, uint32_t frequency = working_frequency) {
  memset(format_ext, 0, sizeof(WAVEFORMATEXTENSIBLE));
  WAVEFORMATEX* format(&format_ext->Format);
  format->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  format->cbSize = 22;
  format->nChannels = 2;
  format->nSamplesPerSec = frequency;
  format->wBitsPerSample = format_ext->Samples.wValidBitsPerSample = 16;
  format->nBlockAlign = (format->nChannels*format->wBitsPerSample)/8;
  format->nAvgBytesPerSec = frequency * format->nBlockAlign;
  memcpy(&format_ext->SubFormat, &PcmSubformatGuid, sizeof(GUID));
}

void Audio::init() {
  REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC/2;

  hr = CoCreateInstance(
    CLSID_MMDeviceEnumerator, NULL,
    CLSCTX_ALL, IID_IMMDeviceEnumerator,
    (void**)&pEnumerator);
  EXIT_ON_ERROR(hr);

  hr = pEnumerator->GetDefaultAudioEndpoint(
    eRender, eConsole, &pDevice);
  EXIT_ON_ERROR(hr);

  hr = pDevice->Activate(
    IID_IAudioClient, CLSCTX_ALL,
    NULL, (void**)&pAudioClient);
  EXIT_ON_ERROR(hr);

  {
    WAVEFORMATEXTENSIBLE wave_format_ext;
    WAVEFORMATEX* wave_format_supported;
    init_wave_format(&wave_format_ext);
    pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)&wave_format_ext, &wave_format_supported);

    if(wave_format_supported && wave_format_supported->nSamplesPerSec!=wave_format_ext.Format.nSamplesPerSec) {
      // Try a second time if the sampling rate wasn't right
      init_wave_format(&wave_format_ext, wave_format_supported->nSamplesPerSec);
      pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)&wave_format_ext, &wave_format_supported);
    }
    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, (WAVEFORMATEX*)&wave_format_ext, NULL);
    EXIT_ON_ERROR(hr);
    internal_freq = wave_format_ext.Format.nSamplesPerSec;
  }

  hr = pAudioClient->GetService(
    IID_IAudioRenderClient,
    (void**)&pRenderClient);
  EXIT_ON_ERROR(hr);

  hr = pAudioClient->Start();  // Start playing.
  EXIT_ON_ERROR(hr)
}
uint32_t Audio::internal_frame_count_ahead() {
  uint32_t wtr;
  hr = pAudioClient->GetCurrentPadding(&wtr);
  return wtr;
}
uint32_t Audio::internal_frequency() {
  return internal_freq;
}
void Audio::internal_write(void* buffer, uint32_t frame_count) {
  BYTE *pData;
  hr = pRenderClient->GetBuffer(frame_count, &pData);
  EXIT_ON_ERROR(hr);

  memcpy(pData, buffer, frame_count*sample_format_size(working_format));

  hr = pRenderClient->ReleaseBuffer(frame_count, 0);
}
