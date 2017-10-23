#include "Audio.h"

#include <Mmdeviceapi.h>
#include <mmreg.h>
#include <mfapi.h>
#include <AudioClient.h>

#include "../macros.h"
#include "../stream/CFileStream.h"

using namespace L;
using namespace Audio;

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { L_BREAKPOINT; }
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

static void init_wave_format(WAVEFORMATEXTENSIBLE * format_ext) {
  memset(format_ext, 0, sizeof(WAVEFORMATEXTENSIBLE));
  WAVEFORMATEX* format(&format_ext->Format);
  format->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  format->cbSize = 22;
  format->nChannels = 2;
  format->nSamplesPerSec = working_frequency;
  format->wBitsPerSample = format_ext->Samples.wValidBitsPerSample = 16;
  format->nBlockAlign = (format->nChannels*format->wBitsPerSample)/8;
  format->nAvgBytesPerSec = working_frequency * format->nBlockAlign;
  memcpy(&format_ext->SubFormat, &PcmSubformatGuid, sizeof(GUID));
}

void Audio::init() {
  REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC/5;

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
    init_wave_format(&wave_format_ext);
    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, (WAVEFORMATEX*)&wave_format_ext, NULL);
    EXIT_ON_ERROR(hr);
  }

  hr = pAudioClient->GetService(
    IID_IAudioRenderClient,
    (void**)&pRenderClient);
  EXIT_ON_ERROR(hr);

  hr = pAudioClient->Start();  // Start playing.
  EXIT_ON_ERROR(hr)
}
uint32_t acquired_frame_count(0);
void Audio::acquire_buffer(void*& buffer, uint32_t& frame_count) {
  L_ASSERT(acquired_frame_count==0);

  // How many frames of advance do we already have?
  uint32_t frame_count_ahead;
  hr = pAudioClient->GetCurrentPadding(&frame_count_ahead);
  EXIT_ON_ERROR(hr);

  // How many frames of advance do we want?
  const Time ideal_time_ahead(ideal_time_ahead());
  const uint32_t ideal_frame_count_ahead(ideal_time_ahead.fSeconds()*working_frequency);
  frame_count = (frame_count_ahead<ideal_frame_count_ahead) ? (ideal_frame_count_ahead-frame_count_ahead) : 0;

  // Get buffer to fill
  BYTE *pData;
  hr = pRenderClient->GetBuffer(frame_count, &pData);
  EXIT_ON_ERROR(hr);
  buffer = pData;
  memset(buffer, 0, frame_count*sample_format_size(working_format));

  acquired_frame_count = frame_count;
}
void Audio::commit_buffer() {
  L_ASSERT(acquired_frame_count>0);
  hr = pRenderClient->ReleaseBuffer(acquired_frame_count, 0);
  EXIT_ON_ERROR(hr);
  acquired_frame_count = 0;
}
