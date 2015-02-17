#include "Midi.h"

using namespace L;
using namespace Audio;

#if defined L_WINDOWS
    Midi::Midi(){
        midiOutOpen(&device, 0, 0, 0, CALLBACK_NULL);
    }
    void Midi::send(byte msgData[4]){
        union{unsigned long word; byte data[4];} message;
        message.data[0] = msgData[0];
        message.data[1] = msgData[1];
        message.data[2] = msgData[2];
        message.data[3] = msgData[3];
        midiOutShortMsg(device, message.word);
    }
#else
    Midi::Midi(){
         fd = open("/dev/midi", O_WRONLY);
    }
    void Midi::send(byte msgData[4]){
        write(fd,msgData,4);
    }
#endif

void Midi::sInstrument(byte channel, byte instrument){
    byte msgData[4];
    msgData[0] = 0xC0 + (channel%16); // Channel changing instrument
    msgData[1] = instrument%128; // New instrument id
    msgData[2] = 0;
    msgData[3] = 0;
    get().send(msgData);
}
void Midi::playNote(byte channel, byte noteNumber, byte velocity){
    byte msgData[4];
    msgData[0] = 0x90 + (channel%16); // Channel playing note
    msgData[1] = noteNumber%128; // Note
    msgData[2] = velocity%128; // Velocity
    msgData[3] = 0;
    get().send(msgData);
}
void Midi::stopNote(byte channel, byte noteNumber){
    byte msgData[4];
    msgData[0] = 0x80 + (channel%16); // Channel stopping note
    msgData[1] = noteNumber%128; // Note
    msgData[2] = 0; // Velocity
    msgData[3] = 0;
    get().send(msgData);
}
void Midi::stopAll(byte channel){
    for(uint t(0);t<128;t++)
        Audio::Midi::stopNote(channel,t);
}
void Midi::stopAll(){
    for(uint c(0);c<16;c++)
        Audio::Midi::stopAll(c);
}
