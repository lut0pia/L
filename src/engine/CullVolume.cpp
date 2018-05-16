#include "CullVolume.h"

#include "../component/Camera.h"
#include "../dev/profiling.h"

using namespace L;

Interval3fTree<CullVolume*> CullVolume::_tree;

void CullVolume::cull(const Camera& camera) {
  L_SCOPE_MARKER("Culling");
  Vector4f planes[6];
  camera.frustum_planes(planes);
  Array<const Node*> stack, invisible;
  if(_tree.root())
    stack.push(_tree.root());
  {
    L_SCOPE_MARKER("Mark visible");
    while(!stack.empty()) {
      const Node* node(stack.back());
      stack.pop();

      // Compute visibility
      const Interval3f& bounds(node->key());
      bool visible(true);
      for(uintptr_t i(0); i<6; i++) {
        const Vector4f& plane(planes[i]);
        if((max(bounds.min().x()*plane.x(), bounds.max().x()*plane.x())
            +max(bounds.min().y()*plane.y(), bounds.max().y()*plane.y())
            +max(bounds.min().z()*plane.z(), bounds.max().z()*plane.z()))+plane.w() < 0.f) {
          visible = false;
          break;
        }
      }

      if(visible) {
        if(node->branch()) {
          stack.push(node->left());
          stack.push(node->right());
        } else
          node->value()->_visible = true;
      } else invisible.push(node);
    }
  }
  {
    L_SCOPE_MARKER("Mark invisible");
    while(!invisible.empty()) {
      const Node* node(invisible.back());
      invisible.pop();
      if(node->branch()) {
        invisible.push(node->left());
        invisible.push(node->right());
      } else
        node->value()->_visible = false;
    }
  }
}
