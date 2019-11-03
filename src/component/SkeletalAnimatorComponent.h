#pragma once

#include "Primitive.h"
#include "../rendering/Animation.h"

namespace L {
  class SkeletalAnimatorComponent : public Component {
    L_COMPONENT(SkeletalAnimatorComponent)
      L_COMPONENT_HAS_ASYNC_LATE_UPDATE(SkeletalAnimatorComponent)
  protected:
    Transform* _transform = nullptr;
    Primitive* _primitive = nullptr;
    Resource<Skeleton> _skeleton;
    Resource<Animation> _animation;
    Array<JointPose> _local_pose;
    Array<Matrix44f> _global_pose;
    Array<Matrix44f> _skin;
    float _time = 0.f;

  public:
    void late_update();
    void update_components();
    static void script_registration();

    inline void skeleton(const char* filename) { _skeleton = filename; }
    inline void animation(const char* filename) { _animation = filename; }
  };
}
