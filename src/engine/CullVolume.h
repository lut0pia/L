#pragma once

#include "../container/IntervalTree.h"

namespace L {
  class Camera;
  class CullVolume {
  protected:
    static Interval3fTree<CullVolume*> _tree;
    typedef Interval3fTree<CullVolume*>::Node Node;
    Node* _node;
    bool _visible;
  public:
    constexpr CullVolume() : _node(nullptr), _visible(false) {}
    inline ~CullVolume() { if(_node) _tree.remove(_node); }
    void update_bounds(const Interval3f& bounds) {
      if(!_node) _node = _tree.insert(bounds, this);
      else if(!_node->key().contains(bounds))
        _tree.update(_node, bounds.extended(1.f));
    }
    inline bool visible() const { return _visible; }

    static void cull(const Camera&);
  };
}
