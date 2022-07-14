#include <L/src/audio/Audio.h>
#include <L/src/audio/Midi.h>
#include <L/src/dev/debug.h>
#include <L/src/engine/Engine.h>
#include <L/src/macros.h>
#include <L/src/system/Memory.h>

L_PUSH_NO_WARNINGS

#define RBN_IMPLEMENTATION
#define RBN_GENERAL_IMPLEMENTATION
#include <robin_general.h>

L_POP_NO_WARNINGS

using namespace L;

static class RobinMidi* robin_midi = nullptr;

class RobinMidi : public Midi {
protected:
  rbn_instance _instance{};
  rbn_output_config _output_config{};

public:
  RobinMidi() {
    rbn_config config{};
    config.sample_rate = Audio::working_frequency;
    rbn_general_init(&_instance, &config);

    switch(Audio::working_format) {
      case Audio::SampleFormat::Stereo16:
        _output_config.sample_format = rbn_sample_format::rbn_s16;
        _output_config.stride = 2;
        break;
      default:
        warning("robin: Unhandled sample format");
        return;
    }

    Engine::add_audio_render([](void* frames, uint32_t frame_count) {
      robin_midi->audio_render(frames, frame_count);
    });
  }

protected:
  void send_internal(const MidiEvent& e) override {
    rbn_msg msg;
    msg.channel = e.msg.channel;
    msg.type = e.msg.type << 4;
    msg.key = e.msg.note;
    msg.velocity = e.msg.velocity;
    rbn_send_msg(&_instance, msg);
  }
  void audio_render(void* frames, uint32_t frame_count) {
    const size_t value_count = frame_count * 2;
    int16_t* tmp = Memory::alloc_type<int16_t>(value_count);

    _output_config.sample_count = frame_count;
    _output_config.left_buffer = tmp;
    _output_config.right_buffer = tmp + 1;
    rbn_render(&_instance, &_output_config);

    for(uintptr_t i = 0; i < value_count; i++) {
      ((int16_t*)frames)[i] += tmp[i];
    }

    Memory::free_type<int16_t>(tmp, value_count);
  }
};

void robin_module_init() {
  robin_midi = Memory::new_type<RobinMidi>();
}
