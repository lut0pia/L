#include "Format.h"

using namespace L;
using namespace Audio;

Format::Format(ushort channels, ushort samplingRate, ushort bytesPerSample)
  : channels(channels), samplingRate(samplingRate), bytesPerSample(bytesPerSample) {}
bool Format::operator==(const Format& other) const {
  return channels == other.channels
         && samplingRate == other.samplingRate
         && bytesPerSample == other.bytesPerSample;
}
bool Format::operator!=(const Format& other) const {
  return !((*this) == other);
}

uint Format::bytesPerSecond() const {
  return channels*samplingRate*bytesPerSample;
}
#if defined L_WINDOWS
WAVEFORMATEX Format::gWaveFormatEx() const {
  WAVEFORMATEX wtr;
  wtr.wFormatTag = WAVE_FORMAT_PCM;
  wtr.nChannels = channels;
  wtr.nSamplesPerSec = samplingRate;
  wtr.nBlockAlign = channels*bytesPerSample;
  wtr.nAvgBytesPerSec = channels*samplingRate*bytesPerSample;
  wtr.wBitsPerSample = bytesPerSample*8;
  wtr.cbSize = 0;
  return wtr;
}
#endif
