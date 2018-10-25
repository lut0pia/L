#include <L/src/audio/Audio.h>
#include <L/src/audio/AudioOutput.h>
#include <L/src/dev/debug.h>
#include <L/src/system/Memory.h>

#include <alsa/asoundlib.h>

using namespace L;
using namespace Audio;

class ALSAOutput : public AudioOutput {
protected:
  snd_pcm_t* _handle;
  uint32_t _buffer_size;
public:
  ALSAOutput(snd_pcm_t* handle)
    : _handle(handle), _buffer_size(snd_pcm_avail_update(_handle)) {
  }
  uint32_t frame_count_ahead() override {
    snd_pcm_sframes_t avail_update(snd_pcm_avail_update(_handle));
    if(avail_update<0) {
      // After an underrun, avail update may return an error code (Broken pipe)
      // In that case we can assume the full buffer is available, meaning we are 0 frames ahead
      avail_update = _buffer_size;
    }
    return _buffer_size-avail_update;
  }
  uint32_t frequency() override { return Audio::working_frequency; }
  void write(void* buffer, uint32_t frame_count) override {
    int rc = snd_pcm_writei(_handle, buffer, frame_count);
    if(rc == -EPIPE) {
      warning("ALSA: Underrun occurred");
      snd_pcm_prepare(_handle);
    } else if(rc < 0) {
      error("ALSA: writei: %s", snd_strerror(rc));
    } else if(rc != frame_count) {
      error("ALSA: short write, wrote %d instead of %d", rc, frame_count);
    }
  }
};

void alsa_module_init() {
  snd_pcm_t* handle;
  snd_pcm_hw_params_t* params;
  snd_pcm_uframes_t frames;

  int dir(0);
  int rc;
  const unsigned int frequency(Audio::working_frequency);

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if(rc < 0) {
    error("ALSA: Unable to open pcm device: %s", snd_strerror(rc));
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);
  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(handle, params, sample_format_channels(working_format));
  snd_pcm_hw_params_set_rate_near(handle, params, &frequency, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if(rc < 0) {
    error("ALSA: Unable to set hw parameters! %s", snd_strerror(rc));
  }

  snd_pcm_hw_params_get_rate(params, &frequency, &dir);

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);

  Memory::new_type<ALSAOutput>(handle);
}
