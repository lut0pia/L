#include <L/src/audio/Audio.h>
#include <L/src/audio/AudioOutput.h>
#include <L/src/dev/debug.h>
#include <L/src/system/Memory.h>

#include <Mmdeviceapi.h>
#include <mmreg.h>
#include <mfapi.h>
#include <AudioClient.h>

using namespace L;

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const GUID PcmSubformatGuid = {STATIC_KSDATAFORMAT_SUBTYPE_PCM};

static void init_wave_format(WAVEFORMATEXTENSIBLE * format_ext, uint32_t frequency = Audio::working_frequency) {
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

class WinAudioOutput : public AudioOutput {
protected:
  IAudioClient* _audio_client = nullptr;
  IAudioRenderClient* _render_client = nullptr;
  uint32_t _frequency;
public:
  WinAudioOutput(IAudioClient* audio_client, IAudioRenderClient* render_client, uint32_t frequency)
    : _audio_client(audio_client), _render_client(render_client), _frequency(frequency) {
  }
  uint32_t frame_count_ahead() override {
    uint32_t wtr;
    _audio_client->GetCurrentPadding(&wtr);
    return wtr;
  }
  uint32_t frequency() override { return _frequency; }
  void write(void* buffer, uint32_t frame_count) override {
    BYTE *client_buffer;
    _render_client->GetBuffer(frame_count, &client_buffer);
    memcpy(client_buffer, buffer, frame_count*Audio::sample_format_size(Audio::working_format));
    _render_client->ReleaseBuffer(frame_count, 0);
  }
};

static bool try_instantiate_audio_output() {
#define CHECKED(...) if(FAILED(__VA_ARGS__)) return false;

  IMMDeviceEnumerator *enumerator = nullptr;
  IMMDevice *device = nullptr;
  IAudioClient *audio_client = nullptr;
  IAudioRenderClient *render_client = nullptr;
  uint32_t frequency;


  CoInitialize(nullptr);

  CHECKED(CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator));
  CHECKED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device));
  CHECKED(device->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&audio_client));

  {
    WAVEFORMATEXTENSIBLE wave_format_ext;
    WAVEFORMATEX* wave_format_supported;
    init_wave_format(&wave_format_ext);
    audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)&wave_format_ext, &wave_format_supported);

    if(wave_format_supported && wave_format_supported->nSamplesPerSec!=wave_format_ext.Format.nSamplesPerSec) {
      // Try a second time if the sampling rate wasn't right
      init_wave_format(&wave_format_ext, wave_format_supported->nSamplesPerSec);
      audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)&wave_format_ext, &wave_format_supported);
    }
    const REFERENCE_TIME half_second = 5000000; // This value's unit is 100ns 
    CHECKED(audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, half_second, 0, (WAVEFORMATEX*)&wave_format_ext, nullptr));
    frequency = wave_format_ext.Format.nSamplesPerSec;
  }

  CHECKED(audio_client->GetService(IID_IAudioRenderClient, (void**)&render_client));
  CHECKED(audio_client->Start());

  Memory::new_type<WinAudioOutput>(audio_client, render_client, frequency);

  CoUninitialize();

  return true;

#undef CHECKED
}

void audio_win_module_init() {
  if(!try_instantiate_audio_output()) {
    warning("Couldn't create audio output, audio may be harmed");
  }
}
