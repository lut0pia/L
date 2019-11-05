#include "Animation.h"

using namespace L;

static inline void find_sample(const AnimationChannel& channel, float time, uintptr_t& i, float& t, float& span) {
  for(i = 0; i < channel.times.size(); i++) {
    if(channel.times[i] > time) {
      if(i == 0) { // Before first sample, take value as is
        t = 0.f;
        span = 0.f;
        return;
      } else { // Interpolate values
        i = i - 1;
        span = channel.times[i + 1] - channel.times[i];
        t = (time - channel.times[i]) / span;
        return;
      }
    } else if(i == channel.times.size() - 1) { // After last sample, take value as is
      t = 0.f;
      span = 0.f;
      return;
    }
  }
}

void Animation::pose_at(float time, JointPose* joints) const {
  for(const AnimationChannel& channel : channels) {
    Vector3f& translation = joints[channel.joint_index].translation;
    Quatf& rotation = joints[channel.joint_index].rotation;
    float& scale = joints[channel.joint_index].scale;

    uintptr_t i;
    float t, span;
    find_sample(channel, time, i, t, span);

    switch(channel.interpolation) {
      case AnimationInterpolationType::Step:
        switch(channel.type) {
          case AnimationChannelType::Translation:
            memcpy(&translation, channel.values.begin() + i * 3, sizeof(Vector3f));
            break;
          case AnimationChannelType::Rotation:
            memcpy(&rotation, channel.values.begin() + i * 4, sizeof(Quatf));
            break;
          case AnimationChannelType::Scale:
            scale = channel.values[i];
            break;
        }
        break;
      case AnimationInterpolationType::Linear:
        switch(channel.type) {
          case AnimationChannelType::Translation:
          {
            Vector3f a, b;
            memcpy(&a, channel.values.begin() + i * 3, sizeof(Vector3f));
            memcpy(&b, channel.values.begin() + (i + 1) * 3, sizeof(Vector3f));
            translation = a * (1.f - t) + b * t;
            break;
          }
          case AnimationChannelType::Rotation:
          {
            Vector4f a, b;
            memcpy(&a, channel.values.begin() + i * 4, sizeof(Vector4f));
            memcpy(&b, channel.values.begin() + (i + 1) * 4, sizeof(Vector4f));
            rotation = a * (1.f - t) + b * t;
            break;
          }
          case AnimationChannelType::Scale:
            scale = channel.values[i] * (1.f - t) + channel.values[i + 1] * t;
            break;
        }
        break;
      case AnimationInterpolationType::CubicSpline:
      {
        const float t2 = t * t;
        const float t3 = t2 * t;
        const float p0f = 2.f * t3 - 3.f * t2 + 1.f;
        const float m0f = t3 - 2.f * t2 + t;
        const float p1f = -2.f * t3 + 3.f * t2;
        const float m1f = t3 - t2;
        switch(channel.type) {
          case AnimationChannelType::Translation:
          {
            Vector3f p0, m0, p1, m1;
            memcpy(&p0, channel.values.begin() + i * 9 + 3, sizeof(Vector3f));
            memcpy(&m0, channel.values.begin() + i * 9 + 6, sizeof(Vector3f));
            memcpy(&p1, channel.values.begin() + (i + 1) * 9 + 3, sizeof(Vector3f));
            memcpy(&m1, channel.values.begin() + (i + 1) * 9, sizeof(Vector3f));
            m0 *= span;
            m1 *= span;
            translation = p0f * p0 + m0f * m0 + p1f * p1 + m1f * m1;
            break;
          }
          case AnimationChannelType::Rotation:
          {
            Vector4f p0, m0, p1, m1;
            memcpy(&p0, channel.values.begin() + i * 12 + 4, sizeof(Vector4f));
            memcpy(&m0, channel.values.begin() + i * 12 + 8, sizeof(Vector4f));
            memcpy(&p1, channel.values.begin() + (i + 1) * 12 + 4, sizeof(Vector4f));
            memcpy(&m1, channel.values.begin() + (i + 1) * 12, sizeof(Vector4f));
            m0 *= span;
            m1 *= span;
            rotation = p0f * p0 + m0f * m0 + p1f * p1 + m1f * m1;
            break;
          }
          case AnimationChannelType::Scale:
            float p0, m0, p1, m1;
            p0 = channel.values[i * 3 + 1];
            m0 = channel.values[i * 3 + 2];
            p1 = channel.values[(i + 1) * 3 + 1];
            m1 = channel.values[(i + 1) * 3];
            m0 *= span;
            m1 *= span;
            scale = p0f * p0 + m0f * m0 + p1f * p1 + m1f * m1;
            break;
        }
        break;
      }
    }
  }
}
