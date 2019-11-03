#include "Animation.h"

using namespace L;

void Animation::pose_at(float time, JointPose* joints) const {
  for(const Channel& channel : channels) {
    float& channel_target = ((float*)&joints[channel.joint_index])[uintptr_t(channel.type)];
    for(uintptr_t i = 0; i < channel.samples.size(); i++) {
      const Sample& sample = channel.samples[i];
      if(sample.time > time) {
        if(i == 0) { // Before first sample, take value as is
          channel_target = sample.value;
          break;
        } else { // Interpolate values
          const Sample& previous_sample = channel.samples[i - 1];
          const float alpha = (time - previous_sample.time) / (sample.time - previous_sample.time);
          channel_target = sample.value * alpha + previous_sample.value * (1.f - alpha);
          break;
        }
      } else if(i == channel.samples.size() - 1) { // After last sample, take value as is
        channel_target = sample.value;
        break;
      }
    }
  }
}
