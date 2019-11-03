#pragma once

#include "../container/Array.h"
#include "../math/Quaternion.h"
#include "../math/Matrix.h"
#include "../stream/serial_bin.h"
#include "../text/Symbol.h"

namespace L {
  struct JointPose {
    Vector3f translation;
    Quatf rotation;
    Vector3f scale;
  };

  struct Animation {
    typedef Animation Intermediate;
    enum class ChannelType : uint8_t {
      TransX, TransY, TransZ,
      RotX, RotY, RotZ, RotW,
      ScaleX, ScaleY, ScaleZ,
    };

    struct Sample {
      float time;
      float value;
    };

    struct Channel {
      Array<Sample> samples;
      Symbol joint_name;
      uintptr_t joint_index;
      ChannelType type;
    };

    Array<Channel> channels;

    void pose_at(float time, JointPose* joints) const;
  };

  struct Skeleton {
    typedef Skeleton Intermediate;
    struct Joint {
      Matrix44f inv_bind_pose;
      Symbol name;
      intptr_t parent;
    };
    Array<Joint> joints;
  };


  inline Stream& operator<=(Stream& s, const Animation::Channel& v) { return s <= v.samples <= v.joint_name <= v.type <= v.joint_index; }
  inline Stream& operator>=(Stream& s, Animation::Channel& v) { return s >= v.samples >= v.joint_name >= v.type >= v.joint_index; }
  inline Stream& operator<=(Stream& s, const Animation& v) { return s <= v.channels; }
  inline Stream& operator>=(Stream& s, Animation& v) { return s >= v.channels; }
  inline Stream& operator<=(Stream& s, const Skeleton::Joint& v) { return s <= v.inv_bind_pose <= v.name <= v.parent; }
  inline Stream& operator>=(Stream& s, Skeleton::Joint& v) { return s >= v.inv_bind_pose >= v.name >= v.parent; }
  inline Stream& operator<=(Stream& s, const Skeleton& v) { return s <= v.joints; }
  inline Stream& operator>=(Stream& s, Skeleton& v) { return s >= v.joints; }
}
