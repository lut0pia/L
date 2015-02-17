#include "Output.h"

#include "../Exception.h"

using namespace L;
using namespace Audio;

Output::Output(const Format& format) : format(format), buffersSem(0){
    #if defined L_WINDOWS
        WAVEFORMATEX wfx(format.gWaveFormatEx());
        if(waveOutOpen(&hwo,WAVE_MAPPER,&wfx,0,0,CALLBACK_NULL))
            throw Exception("Audio output open failed.");
        buffersThread.start(buffersFunc,this);
    #else
        throw Exception("Audio output not implemented on this system.");
    #endif
}
Output::~Output(){
    #if defined L_WINDOWS
        waveOutClose(hwo);
    #endif
}

void Output::write(const Ref<Buffer>& buffer){
    if(buffer->gFormat()!=format)
        throw Exception("Audio output format different from buffer format.");
    #if defined L_WINDOWS
        WAVEHDR wh = {(LPSTR)buffer->gBytes(),buffer->size(),0,0,0,0};
        if(waveOutPrepareHeader(hwo,&wh,sizeof(WAVEHDR))
        || waveOutWrite(hwo,&wh,sizeof(WAVEHDR)))
            throw Exception("Audio output header failed.");

        while(waveOutUnprepareHeader(hwo,&wh,sizeof(WAVEHDR)) == WAVERR_STILLPLAYING){};
    #endif
}

Output& Output::operator<<(const Ref<Buffer>& buffer){
    buffersMutex.lock();
        buffers.push_back(buffer);
    buffersMutex.unlock();
    buffersSem.post();
    return *this;
}

Dynamic::Var Output::buffersFunc(Thread* t){
    Output& o(*t->gArg().as<Output*>());
    while(true){
        o.buffersSem.wait();
        o.buffersMutex.lock();
            if(o.buffers.empty()){
                o.buffersMutex.unlock();
                break;
            }
            Ref<Buffer> buffer(o.buffers.front());
            o.buffers.pop_front();
        o.buffersMutex.unlock();
        o.write(buffer);
    }
    return Dynamic::Var();
}

