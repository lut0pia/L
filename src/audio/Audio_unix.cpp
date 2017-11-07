#include "Audio.h"

using namespace L;
using namespace Audio;

void Audio::init() {
}
uint32_t acquired_frame_count(0);
void Audio::acquire_buffer(void*& buffer, uint32_t& frame_count) {
  frame_count = 0;
}
void Audio::commit_buffer() {
}
