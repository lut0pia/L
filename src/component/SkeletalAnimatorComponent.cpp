#include "SkeletalAnimatorComponent.h"

#include "../engine/debug_draw.h"
#include "../engine/Engine.h"
#include "../engine/Settings.h"
#include "../engine/Resource.inl"

using namespace L;

void SkeletalAnimatorComponent::late_update() {
  if(_skeleton && _animation) {
    const Skeleton& skeleton(*_skeleton);
    const Animation& animation(*_animation);

    // Reset local pose
    _local_pose.size(skeleton.joints.size());
    for(JointPose& local_pose : _local_pose) {
      local_pose.translation = 0.f;
      local_pose.rotation = Quatf();
      local_pose.scale = 1.f;
    }

    // Compute local pose
    animation.pose_at(_time, _local_pose.begin());

    // Compute global pose
    _global_pose.size(_local_pose.size());
    for(uintptr_t i = 0; i < _global_pose.size(); i++) {
      const uintptr_t parent = skeleton.joints[i].parent;
      const JointPose& local_pose = _local_pose[i];
      const Matrix44f matrix = sqt_to_mat(local_pose.rotation, local_pose.translation, local_pose.scale);
      L_ASSERT(parent < i || parent == UINTPTR_MAX);
      if(parent == UINTPTR_MAX) { // Parent is root
        _global_pose[i] = matrix;
      } else {
        _global_pose[i] = _global_pose[parent] * matrix;
      }
    }

    // Compute skinning matrices
    _skin.size(_global_pose.size());
    for(uintptr_t i(0); i < _skin.size(); i++) {
      _skin[i] = _global_pose[i] * skeleton.joints[i].inv_bind_pose;
    }

    _primitive->material()->set_buffer("Pose", _skin.begin(), sizeof(Matrix44f) * _skin.size());

#if L_DEBUG
    if(Settings::get_int("render_skeletal_animator", 0)) {
      for(uintptr_t i = 0; i < skeleton.joints.size(); i++) {
        const SkeletonJoint& joint = skeleton.joints[i];
        const float bounds_length = _primitive->material()->bounds().size().length();
        const Vector3f offset = Vector3f(0.f, bounds_length, 0.f);
        const float debug_scale = bounds_length / 32.f;
        const Vector3f joint_pos = _global_pose[i].vector<3>(3) + offset;
        const Vector3f joint_x = _global_pose[i].vector<3>(0) * debug_scale;
        const Vector3f joint_y = _global_pose[i].vector<3>(1) * debug_scale;
        const Vector3f joint_z = _global_pose[i].vector<3>(2) * debug_scale;

        debug_draw_line(_transform->to_absolute(joint_pos), _transform->to_absolute(joint_pos + joint_x), Color::red);
        debug_draw_line(_transform->to_absolute(joint_pos), _transform->to_absolute(joint_pos + joint_y), Color::green);
        debug_draw_line(_transform->to_absolute(joint_pos), _transform->to_absolute(joint_pos + joint_z), Color::blue);

        const Matrix44f skel_bind_pose = joint.inv_bind_pose.inverse();
        const Vector3f skel_pos = skel_bind_pose.vector<3>(3) + offset * 2.f;
        const Vector3f skel_x = skel_bind_pose.vector<3>(0) * debug_scale;
        const Vector3f skel_y = skel_bind_pose.vector<3>(1) * debug_scale;
        const Vector3f skel_z = skel_bind_pose.vector<3>(2) * debug_scale;

        debug_draw_line(_transform->to_absolute(skel_pos), _transform->to_absolute(skel_pos + skel_x), Color::red);
        debug_draw_line(_transform->to_absolute(skel_pos), _transform->to_absolute(skel_pos + skel_y), Color::green);
        debug_draw_line(_transform->to_absolute(skel_pos), _transform->to_absolute(skel_pos + skel_z), Color::blue);

        if(joint.parent >= 0) {
          const Vector3f joint_parent_pos = _global_pose[joint.parent].vector<3>(3) + offset;
          debug_draw_line(_transform->to_absolute(joint_parent_pos), _transform->to_absolute(joint_pos), Color::from_index(i));
          const Vector3f skel_parent_pos = skeleton.joints[joint.parent].inv_bind_pose.inverse().vector<3>(3) + offset * 2.f;
          debug_draw_line(_transform->to_absolute(skel_parent_pos), _transform->to_absolute(skel_pos), Color::from_index(i));
        } else {
          debug_draw_line(_transform->to_absolute(offset), _transform->to_absolute(joint_pos), Color::white);
          debug_draw_line(_transform->to_absolute(offset * 2.f), _transform->to_absolute(skel_pos), Color::white);
        }
      }
    }
#endif
  }
  _time = fmod(_time + Engine::delta_seconds(), 3.f);
}
void SkeletalAnimatorComponent::update_components() {
  _transform = entity()->require_component<Transform>();
  _primitive = entity()->require_component<Primitive>();
}
void SkeletalAnimatorComponent::script_registration() {
  L_COMPONENT_BIND(SkeletalAnimatorComponent, "skeletal_animator");
  L_SCRIPT_METHOD(SkeletalAnimatorComponent, "skeleton", 1, skeleton(c.param(0).get<String>()));
  L_SCRIPT_METHOD(SkeletalAnimatorComponent, "animation", 1, animation(c.param(0).get<String>()));
}
