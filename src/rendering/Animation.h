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
    float scale;
  };

  enum class AnimationChannelType : uint8_t {
    Translation, Rotation, Scale,
  };

  enum class AnimationInterpolationType : uint8_t {
    Step, Linear, CubicSpline,
  };

  struct AnimationChannel {
    Array<float> times;
    Array<float> values;
    Symbol joint_name;
    uintptr_t joint_index;
    AnimationChannelType type;
    AnimationInterpolationType interpolation;
  };

  struct Animation {
    typedef Animation Intermediate;

    Array<AnimationChannel> channels;
    float duration;

    void pose_at(float time, JointPose* joints) const;
  };

  struct SkeletonJoint {
    Matrix44f inv_bind_pose;
    Symbol name;
    intptr_t parent;
  };

  struct Skeleton {
    typedef Skeleton Intermediate;

    Array<SkeletonJoint> joints;
  };


  inline Stream& operator<=(Stream& s, const AnimationChannel& v) { return s <= v.times <= v.values <= v.joint_name <= v.joint_index <= v.type <= v.interpolation; }
  inline Stream& operator>=(Stream& s, AnimationChannel& v) { return s >= v.times >= v.values >= v.joint_name >= v.joint_index >= v.type >= v.interpolation; }
  inline size_t get_cpu_size(const AnimationChannel& v) { return get_cpu_size(v.times) + get_cpu_size(v.values); }
  inline void resource_write(Stream& s, const Animation& v) { s <= v.channels <= v.duration; }
  inline void resource_read(Stream& s, Animation& v) { s >= v.channels >= v.duration; }
  inline size_t resource_cpu_size(const Animation& v) { return get_cpu_size(v.channels); }
  inline Stream& operator<=(Stream& s, const SkeletonJoint& v) { return s <= v.inv_bind_pose <= v.name <= v.parent; }
  inline Stream& operator>=(Stream& s, SkeletonJoint& v) { return s >= v.inv_bind_pose >= v.name >= v.parent; }
  inline void resource_write(Stream& s, const Skeleton& v) { s <= v.joints; }
  inline void resource_read(Stream& s, Skeleton& v) { s >= v.joints; }
  inline size_t resource_cpu_size(const Skeleton& v) { return get_cpu_size(v.joints); }
}
