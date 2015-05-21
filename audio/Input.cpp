#include "Input.h"

#include "../Exception.h"

using namespace L;
using namespace Audio;

Input::Input(const Format& format) : format(format) {
#if defined L_WINDOWS
  WAVEFORMATEX wfx(format.gWaveFormatEx());
  if(waveInOpen(&hwi,WAVE_MAPPER,&wfx,0,0,CALLBACK_NULL))
    throw Exception("Audio input open failed.");
  if(waveInStart(hwi))
    throw Exception("Audio input start failed.");
#else
  throw Exception("Audio input not implemented on this system.");
#endif
}
Input::~Input() {
#if defined L_WINDOWS
  waveInClose(hwi);
#endif
}
void Input::read(const Ref<Buffer>& buffer) {
  if(format!=buffer->gFormat())
    throw Exception("Audio input format different from buffer format.");
#if defined L_WINDOWS
  WAVEHDR wh = {(LPSTR)buffer->gBytes(),buffer->size(),0,0,0,0};
  if(waveInPrepareHeader(hwi,&wh,sizeof(WAVEHDR))
      || waveInAddBuffer(hwi,&wh,sizeof(WAVEHDR)))
    throw Exception("Audio input header failed.");
  while(waveInUnprepareHeader(hwi,&wh,sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {};
#endif
}
