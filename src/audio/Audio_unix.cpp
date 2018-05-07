#include "Audio.h"

#include <alsa/asoundlib.h>

#include "../dev/debug.h"
#include "../system/Memory.h"

using namespace L;
using namespace Audio;

snd_pcm_t* handle;
snd_pcm_hw_params_t* params;
snd_pcm_uframes_t frames;
uint32_t buffer_size;

unsigned int internal_freq(working_frequency);

void Audio::init() {
  unsigned int val;
  int dir(0);
  int rc;

  printf("ALSA: %s\n", SND_LIB_VERSION_STR);

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    error("ALSA: Unable to open pcm device: %s",snd_strerror(rc));
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);
  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(handle, params, sample_format_channels(working_format));
  snd_pcm_hw_params_set_rate_near(handle, params, &internal_freq, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    error("ALSA: Unable to set hw parameters! %s",snd_strerror(rc));
  }

  snd_pcm_hw_params_get_rate(params, &internal_freq, &dir);

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);

  buffer_size = snd_pcm_avail_update(handle);
}
uint32_t Audio::internal_frame_count_ahead() {
  snd_pcm_sframes_t avail_update(snd_pcm_avail_update(handle));
  if(avail_update<0) {
    // After an underrun, avail update may return an error code (Broken pipe)
    // In that case we can assume the full buffer is available, meaning we are 0 frames ahead
    avail_update = buffer_size;
  }
  return buffer_size-avail_update;
}
uint32_t Audio::internal_frequency() {
  return internal_freq;
}
void Audio::internal_write(void* buffer, uint32_t frame_count) {
  int rc = snd_pcm_writei(handle, buffer, frame_count);
  if(rc == -EPIPE) {
    warning("ALSA: Underrun occurred");
    snd_pcm_prepare(handle);
  } else if(rc < 0) {
    error("ALSA: writei: %s", snd_strerror(rc));
  } else if(rc != frame_count) {
    error("ALSA: short write, wrote %d instead of %d", rc, frame_count);
  }
}
