#pragma once

#include <L/src/L.h>

namespace L {
  class MIDI : public Interface<Audio::MidiSequence> {
    static MIDI instance;
  public:
    MIDI() : Interface{"mid","midi"} {}

    Ref<Audio::MidiSequence> from(const byte* data, size_t size) override {
      if(size<18 || memcmp(data, "MThd", 4))
        return nullptr;

      const uint32_t header_length(read_int(data+4)),
        format(read_int(data+8, 2)),
        track_count(read_int(data+10, 2)),
        division(read_int(data+12, 2));

      if(format>1 || track_count>=32)
        return nullptr;

      Ref<Audio::MidiSequence> wtr(ref<Audio::MidiSequence>());
      const byte* track_heads[32];
      uint32_t current_times[32]{};
      uint8_t previous_commands[32]{};
      uint32_t global_current_time(0);
      uint32_t us_per_tick(500000/division);

      { // Gather track heads
        const byte* head(data+14);
        for(uint32_t track(0); track<track_count; track++) {
          if(memcmp(head, "MTrk", 4))
            return nullptr;
          const uint32_t track_length(read_int(head+4));
          track_heads[track] = head+8;
          head += track_length+8;
        }
      }
      while(true) {
        // Find track with next event
        uint32_t smallest_time(-1);
        uint32_t best_track;
        for(uint32_t track(0); track<track_count; track++) {
          const byte* head(track_heads[track]);
          if(!head) continue; // Track is over

          const uint32_t current_time(current_times[track]);
          const uint32_t delta_time(read_variable(head));
          const uint32_t new_time(current_time+delta_time);
          if(new_time<smallest_time) {
            best_track = track;
            smallest_time = new_time;
          }
        }
        if(smallest_time==-1) // All tracks are over
          break;

        // Add event from best track
        const byte*& head(track_heads[best_track]);
        uint32_t& current_time(current_times[best_track]);
        uint8_t& previous_command(previous_commands[best_track]);
        const uint32_t delta_time(read_variable(head));
        current_time += delta_time;
        const uint32_t global_delta_time(smallest_time-global_current_time);
        global_current_time = smallest_time;
        if(*head==0xff) { // Meta event
          head++;
          const uint8_t type(*head++);
          const uint32_t data_length(read_variable(head));
          switch(type) {
            case 0x2f: // End of track
              head = nullptr;
              break;
            case 0x51: // Tempo setting
            {
              const uint32_t us_per_beat(read_int(head, 3));
              us_per_tick = us_per_beat / division;
              head += data_length;
            } break;
            default:
              head += data_length; // Ignore data
              break;
          }
        } else if(*head>=0xf0) {
          return nullptr;
        } else {
          uint8_t event_type(*head++);
          if(event_type<0x80) { // Running status
            L_ASSERT(previous_command!=0);
            event_type = previous_command;
            head--;
          } else previous_command = event_type;
          size_t event_size;
          switch(event_type&0xf0) {
            case 0x80: case 0x90: case 0xa0: case 0xb0: case 0xe0:
              event_size = 2;
              break;
            case 0xc0: case 0xd0:
              event_size = 1; break;
            default: return nullptr;
          }

          Audio::MidiEvent new_event{};
          new_event.array[0] = event_type;
          for(uintptr_t i(0); i<event_size; i++)
            new_event.array[i+1] = *head++;
          wtr->events.push(Audio::MidiSequence::Event{
            global_delta_time*us_per_tick,
            new_event,
          });
        }
      }

      return wtr;
    }
    static uint32_t read_int(const byte* data, size_t size = 4) {
      uint32_t wtr(0);
      switch(size) {
        case 4: wtr |= uint32_t(*data++);
          wtr <<= 8;
        case 3: wtr |= uint32_t(*data++);
          wtr <<= 8;
        case 2: wtr |= uint32_t(*data++);
          wtr <<= 8;
        case 1: wtr |= uint32_t(*data);
      }
      return wtr;
    }
    static uint32_t read_variable(const byte*& data) {
      uint32_t wtr(0);
      while(true) {
        const uint8_t datum(*data++);
        wtr |= datum&0x7f;
        if(datum&0x80)
          wtr <<= 7;
        else
          break;
      }
      return wtr;
    }
  };
  MIDI MIDI::instance;
}
