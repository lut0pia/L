#include "Collider.h"

#include "../rendering/GL.h"
#include "../rendering/Mesh.h"
#include "../rendering/Program.h"
#include "../math/geometry.h"
#include "../parallelism/TaskSystem.h"

using namespace L;

Interval3fTree<Collider*> Collider::tree;

Collider::Collider() : _node(nullptr),_center(0.f),_radius(1.f),_type(Sphere){}
Collider::~Collider(){
  if(_node)
    tree.remove(_node);
}

void Collider::update_components(){
  _transform = entity()->requireComponent<Transform>();
  _rigidbody = entity()->component<RigidBody>();
  _script = entity()->component<ScriptComponent>();
  if(!_node)
    _node = tree.insert(_boundingBox,this);
}
Map<Symbol, Var> Collider::pack() const {
  Map<Symbol, Var> data;
  data["type"] = Symbol(_type==Box ? "box" : "sphere");
  data["center"] = _center;
  data["radius"] = _radius;
  return data;
}
void Collider::unpack(const Map<Symbol, Var>& data) {
  {
    Symbol type;
    unpack_item(data, "type", type);
    if(type==Symbol("sphere"))
      _type = Sphere;
    else if(type==Symbol("box"))
      _type = Box;
  }
  unpack_item(data, "center", _center);
  unpack_item(data, "radius", _radius);
}
void Collider::script_registration() {
  L_COMPONENT_BIND(Collider, "collider");
  L_COMPONENT_METHOD(Collider, "center", 1, center(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Collider, "box", 1, box(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Collider, "sphere", 1, sphere(c.local(0).get<float>()));
  Script::Context::global(Symbol("raycast")) = (Script::Function)([](Script::Context& c) {
    if(c.localCount()==2 && c.local(0).is<Vector3f>() && c.local(1).is<Vector3f>()) {
      auto wtr(ref<Table<Var, Var>>());
      float t;
      (*wtr)[Symbol("collider")] = Collider::raycast(c.local(0).as<Vector3f>(), c.local(1).as<Vector3f>(), t);
      (*wtr)[Symbol("t")] = t;
      (*wtr)[Symbol("position")] = c.local(0).as<Vector3f>()+c.local(1).as<Vector3f>()*t;
      c.returnValue() = wtr;
    }
  });
}

void Collider::sub_update_all() {
  const uintptr_t thread_count = TaskSystem::thread_count();
  // Update tree nodes
  ComponentPool<Collider>::iterate([](Collider& c) {
    c.updateBoundingBox();
    const Interval3f& bb(c._boundingBox);
    if(!c._node->key().contains(bb))
      tree.update(c._node, bb.extended(c._radius.x()));
  });

  // Collision: broad phase
  static Array<Interval3fTree<Collider*>::Node*> pairs[TaskSystem::max_thread_count];
  static Array<Interval3fTree<Collider*>::Node*> tmp[TaskSystem::max_thread_count];
  for(uintptr_t t(0); t<thread_count; t++)
    pairs[t].clear();
  ComponentPool<Collider>::async_iterate([](Collider& c, uint32_t t) {
    tree.query(c._boundingBox, tmp[t]);
    for(auto e : tmp[t])
      if(e!=c._node && e < c._node)
        pairs[t].pushMultiple(c._node, e);
  });

  // Collision: narrow phase
  static Array<Collision> collisions[TaskSystem::max_thread_count];
  for(uintptr_t i(0); i<thread_count; i++)
    collisions[i].size(pairs[i].size()/2);
  for(uintptr_t t(0); t<thread_count; t++)
    TaskSystem::push([](void* p) {
    const uintptr_t t((uintptr_t)p);
    for(uintptr_t i(0), j(0); i<collisions[t].size(); i++, j += 2) {
      auto &a(pairs[t][j]), &b(pairs[t][j+1]);
      if(!a->value()->_rigidbody)
        swap(a, b); // Rigidbody is always first argument
      checkCollision(*a->value(), *b->value(), collisions[t][i]);
    }
  }, (void*)t);
  TaskSystem::join();

  // Apply all collisions
  for(uintptr_t t(0); t<thread_count; t++)
    for(uintptr_t i(0); i<collisions[t].size(); i++) {
      const Collision& collision(collisions[t][i]);
      if(!collision.colliding)
        continue;
      Collider *a(pairs[t][i*2]->value()), *b(pairs[t][i*2+1]->value());

      // Resolve interpenetration
      if(b->_rigidbody) {
        a->_transform->move_absolute(collision.normal*(collision.overlap*.5f));
        b->_transform->move_absolute(collision.normal*(collision.overlap*-.5f));
      } else a->_transform->move_absolute(collision.normal*collision.overlap);

      // Send collision events to scripts
      if(a->_script || b->_script) {
        auto e(ref<Table<Var, Var>>());
        (*e)[Symbol("type")] = Symbol("COLLISION");
        (*e)[Symbol("point")] = collision.point;
        (*e)[Symbol("overlap")] = collision.overlap;
        if(a->_script) {
          (*e)[Symbol("other")] = b;
          (*e)[Symbol("normal")] = collision.normal;
          a->_script->event(e);
        }
        if(b->_script) {
          (*e)[Symbol("other")] = a;
          (*e)[Symbol("normal")] = -collision.normal;
          b->_script->event(e);
        }
      }

      // Physically resolve collision
      RigidBody::collision(a->_rigidbody, b->_rigidbody, collision.point, collision.normal);
    }
}
static void draw_tree_node(const Interval3fTree<Collider*>::Node* node) {
  static int level(0);
  Program::default_color().use();
  Program::default_color().uniform("color", Color::fromHSV(pmod(level*15.f, 360.f), .5f, 1.f));
  Program::default_color().uniform("model", translation_matrix(node->key().center())*scale_matrix(node->key().size()*.5f));
  Mesh::wire_cube().draw();
  if(node->branch()) {
    level++;
    draw_tree_node(node->left());
    draw_tree_node(node->right());
    level--;
  }
}
void Collider::render_all(const Camera& cam) {
  if(Settings::get_int("render_collider", 0))
    draw_tree_node(tree.root());
}
void Collider::center(const Vector3f& center){
  _center = center;
  if(_rigidbody)
    _rigidbody->updateInertiaTensor();
}
void Collider::box(const Vector3f& radius) {
  _type = Box;
  _radius = radius;
  if(_rigidbody)
    _rigidbody->updateInertiaTensor();
}
void Collider::sphere(float radius) {
  _type = Sphere;
  _radius = radius;
  if(_rigidbody)
    _rigidbody->updateInertiaTensor();
}
void Collider::updateBoundingBox() {
  const Vector3f center(_transform->toAbsolute(_center));
  switch(_type) {
    case Box:
    {
      const Vector3f right(_transform->right()*_radius.x()),forward(_transform->forward()*_radius.y()),up(_transform->up()*_radius.z());
      _boundingBox = (center-right-forward-up);
      _boundingBox.add(center-right-forward+up);
      _boundingBox.add(center-right+forward-up);
      _boundingBox.add(center-right+forward+up);
      _boundingBox.add(center+right-forward-up);
      _boundingBox.add(center+right-forward+up);
      _boundingBox.add(center+right+forward-up);
      _boundingBox.add(center+right+forward+up);
      break;
    }
    case Sphere:
      _boundingBox = Interval3f(center-_radius,center+_radius);
      break;
  }
}
bool Collider::raycastSingle(const Vector3f& origin,const Vector3f& direction,float& t) const{
  switch(_type){
    case Box:
      return rayBoxIntersect(
        Interval3f(_center-_radius,_center+_radius),
        _transform->fromAbsolute(origin),
        _transform->rotation().inverse().rotate(direction),t);
      break;
    case Sphere:
      return raySphereIntersect(_transform->toAbsolute(_center),_radius.x(),origin,direction,t);
      break;
  }
  return false;
}
Matrix33f Collider::inertiaTensor() const{
  Matrix33f wtr(0.f);
  switch(_type){
    case Box:
    {
      const Vector3f diameter(_radius*2.f);
      wtr(0, 0) = (sqr(diameter.y())+sqr(diameter.z()))*(1.f/12.f);
      wtr(1, 1) = (sqr(diameter.x())+sqr(diameter.z()))*(1.f/12.f);
      wtr(2, 2) = (sqr(diameter.x())+sqr(diameter.y()))*(1.f/12.f);
      break;
    }
    case Sphere:
      wtr(0,0) = wtr(1,1) = wtr(2,2) = sqr(_radius.x())*(2.f/5.f);
      break;
  }
  return wtr;
}
void Collider::render(const Camera& camera) {
  Program::default().use();
  Program::default().uniform("model",_transform->matrix()*scale_matrix(_radius));
  switch(_type){
    case Box:
      Mesh::wire_cube().draw();
      break;
    case Sphere:
      Mesh::wire_sphere().draw();
      break;
  }
}
template <size_t count>
Interval1f project(const Vector3f& axis, const Vector3f* points) {
  Interval1f wtr(Interval1f(axis.dot(points[0])));
  for(uintptr_t i(1); i<count; i++)
    wtr.add(axis.dot(points[i]));
  return wtr;
}
template <size_t count>
Vector3f least_to_axis(const Vector3f& axis, const Vector3f* points) {
  uintptr_t least_index(0);
  float least_projected(axis.dot(points[0]));
  for(uintptr_t i(1); i<count; i++) {
    const float projected(axis.dot(points[i]));
    if(projected<least_projected) {
      least_index = i;
      least_projected = projected;
    }
  }
  return points[least_index];
}
bool Collider::checkCollision(const Collider& a,const Collider& b, Collision& collision) {
  if(a.entity()==b.entity() || !a._boundingBox.overlaps(b._boundingBox) || !a._rigidbody)
    return collision.colliding = false;

  if(a._type==Sphere && b._type==Sphere){
    const Vector3f apos(a._transform->toAbsolute(a._center)),
      bpos(b._transform->toAbsolute(b._center)),
      btoa(apos-bpos);
    const float distance(btoa.length());
    collision.overlap = (a._radius.x()+b._radius.x())-distance;
    if(collision.overlap>.0f){
      collision.normal = btoa.normalized();
      collision.point = bpos+collision.normal*b._radius.x();
    } else return collision.colliding = false;
  } else if(a._type==Box && b._type==Box) {
    const Transform *at(a._transform),*bt(b._transform);
    const Vector3f ar(at->right()),af(at->forward()),au(at->up()),
      br(bt->right()),bf(bt->forward()),bu(bt->up());
    const Vector3f axes[] = {
      ar,af,au,br,bf,bu,
      ar.cross(br).normalized(),
      ar.cross(bf).normalized(),
      ar.cross(bu).normalized(),
      af.cross(br).normalized(),
      af.cross(bf).normalized(),
      af.cross(bu).normalized(),
      au.cross(br).normalized(),
      au.cross(bf).normalized(),
      au.cross(bu).normalized()
    };
    const Vector3f apoints[] = {
      at->toAbsolute(a._center+Vector3f(-a._radius.x(),-a._radius.y(),-a._radius.z())),
      at->toAbsolute(a._center+Vector3f(-a._radius.x(),-a._radius.y(),a._radius.z())),
      at->toAbsolute(a._center+Vector3f(-a._radius.x(),a._radius.y(),-a._radius.z())),
      at->toAbsolute(a._center+Vector3f(-a._radius.x(),a._radius.y(),a._radius.z())),
      at->toAbsolute(a._center+Vector3f(a._radius.x(),-a._radius.y(),-a._radius.z())),
      at->toAbsolute(a._center+Vector3f(a._radius.x(),-a._radius.y(),a._radius.z())),
      at->toAbsolute(a._center+Vector3f(a._radius.x(),a._radius.y(),-a._radius.z())),
      at->toAbsolute(a._center+Vector3f(a._radius.x(),a._radius.y(),a._radius.z())),
    };
    const Vector3f bpoints[] = {
      bt->toAbsolute(b._center+Vector3f(-b._radius.x(),-b._radius.y(),-b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(-b._radius.x(),-b._radius.y(),b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(-b._radius.x(),b._radius.y(),-b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(-b._radius.x(),b._radius.y(),b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(b._radius.x(),-b._radius.y(),-b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(b._radius.x(),-b._radius.y(),b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(b._radius.x(),b._radius.y(),-b._radius.z())),
      bt->toAbsolute(b._center+Vector3f(b._radius.x(),b._radius.y(),b._radius.z())),
    };
    uintptr_t axis(sizeof(axes));
    collision.overlap = 0.f;
    for(uintptr_t i(0); i<sizeof(axes)/sizeof(Vector3f); i++) {
      if(axes[i].lengthSquared()>0.00001f) { // The axis is not a null vector (caused by a cross product)
        Interval1f axisA(project<8>(axes[i], apoints)), axisB(project<8>(axes[i], bpoints)), intersection(axisA, axisB); // Compute projections and intersection
        const float overlap(intersection.size().x());
        if(overlap>0.f) {
          if(axis==sizeof(axes) || overlap<collision.overlap) { // First or smallest overlap yet
            collision.normal = (axisA.center().x()<axisB.center().x()) ? -axes[i] : axes[i];
            collision.overlap = overlap;
            axis = i;
          }
        } else return collision.colliding = false; // No overlap means no collision
      }
    }
    // Compute impact point
    if(axis<6)
      collision.point = (axis<3) ? least_to_axis<8>(-collision.normal, bpoints) : least_to_axis<8>(collision.normal, apoints);
    else{
      Vector3f avertex(least_to_axis<8>(collision.normal, apoints)), bvertex(least_to_axis<8>(-collision.normal, bpoints));
      const Vector3f& aaxis(axes[(axis-6)/3]),baxis(axes[((axis-6)%3)+3]); // Find axes used in cross product
      if(!lineLineIntersect(avertex,avertex+aaxis,bvertex,bvertex+baxis,&avertex,&bvertex))
        return collision.colliding = false; // Unable to compute intersection
      collision.point = (avertex+bvertex)/2.f;
    }
  } else{ // Box-Sphere
    const Collider *box,*sphere;
    if(a._type==Box){
      box = &a;
      sphere = &b;
    } else{
      box = &b;
      sphere = &a;
    }
    const Vector3f sphereCenter(sphere->_transform->toAbsolute(sphere->_center));
    const Vector3f relCenter(box->_transform->fromAbsolute(sphereCenter));
    const Vector3f closest(clamp(relCenter,-box->_radius,box->_radius));
    if(relCenter == closest){ // The sphere's center is in the box
      const Vector3f dist(abs(box->_radius-relCenter)); // Distance to box border
      if(dist.x() < dist.y() && dist.x() < dist.z()){
        collision.normal = (box==&a) ? Vector3f(1.f,0,0) : Vector3f(-1.f,0,0);
        collision.overlap = dist.x();
      } else if(dist.y() < dist.x() && dist.y() < dist.z()){
        collision.normal = (box==&a) ? Vector3f(0,1.f,0) : Vector3f(0,-1.f,0);
        collision.overlap = dist.y();
      } else {
        collision.normal = (box==&a) ? Vector3f(0,0,1.f) : Vector3f(0,0,-1.f);
        collision.overlap = dist.z();
      }
      collision.normal = box->_transform->toAbsolute(collision.normal);
    } else {
      collision.point = box->_transform->toAbsolute(closest);
      collision.overlap = sphere->_radius.x()-collision.point.dist(sphereCenter);
      if(collision.overlap>.0f){
        collision.normal = (box==&a) ? collision.point-sphereCenter : sphereCenter-collision.point;
        collision.normal.normalize();
      } else return collision.colliding = false; // No collision
    }
  }
  return collision.colliding = true;
}
Collider* Collider::raycast(const Vector3f& origin,Vector3f direction,float& t){
  typedef Interval3fTree<Collider*>::Node Node;
  static Array<const Node*> queue;
  queue.clear();
  if(tree.root())
    queue.push(tree.root());

  direction.normalize();
  const Vector3f invDir(1.f/direction.x(),1.f/direction.y(),1.f/direction.z());
  Collider* wtr(nullptr);
  while(!queue.empty()){
    const Node* node(queue.back());
    queue.pop();
    float hitT;
    const Interval3f& aabb(node->leaf() ? node->value()->_boundingBox : node->key());
    if(rayBoxIntersect(aabb,origin,direction,hitT,invDir)){
      if(wtr && t<hitT) // Already have closer hit
        continue;
      if(node->leaf()){
        if(node->value()->raycastSingle(origin,direction,hitT)){
          wtr = node->value();
          t = hitT;
        }
      } else {
        queue.push(node->left());
        queue.push(node->right());
      }
    }
  }
  return wtr;
}
