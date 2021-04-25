#include <L/src/container/Array.h>
#include <L/src/container/Handle.h>
#include <L/src/container/Table.h>
#include <L/src/script/script_binding.h>

#include "include/grid2d.h"

using namespace L;

Array<Vector2i> Grid2d::get_neighbors(const Vector2i& p) {
  return {
    Vector2i(p.x(), p.y() + 1),
    Vector2i(p.x(), p.y() - 1),
    Vector2i(p.x() + 1, p.y()),
    Vector2i(p.x() - 1, p.y()),
  };
}
float Grid2d::get_estimated_cost(const Vector2i& from, const Vector2i& to) {
  return float((from - to).manhattan());
}
float Grid2d::get_cost(const Vector2i& from, const Vector2i& to) {
  // TODO: Handle varying costs and obstacles
  return get_estimated_cost(from, to);
}

void Grid2d::set_obstacle(const L::Vector2i& position, bool is_obstacle) {
  _nodes[position].is_obstacle = is_obstacle;
}
void Grid2d::set_edge(const Vector2i& start, const Vector2i& end, float cost) {
  Node& start_node = _nodes[start];
  Node& end_node = _nodes[end];
  start_node.neighbors[end] = cost;
  end_node.neighbors[start] = cost;
}
Array<Vector2i> Grid2d::find_path(const Parameters& parameters) {
  {
    const Node& target_node = _nodes[parameters.target];
    if(target_node.is_obstacle) {
      return Array<Vector2i>();
    }
  }

  { // Reset node statuses
    for(auto& node : _nodes) {
      node.value().status = NodeStatus::New;
    }
  }

  { // Setup start node
    Node& start_node = _nodes[parameters.start];
    start_node.g_score = 0.f;
    start_node.f_score = get_estimated_cost(parameters.start, parameters.target);
    start_node.status = NodeStatus::Open;
  }

  while(true) {
    Table<Vector2i, Node>::Slot current(0, Vector2i());
    for(const auto& node : _nodes) {
      if(node.value().status == NodeStatus::Open &&
        (current.empty() || node.value().f_score < current.value().f_score)) {
        current = node;
      }
    }

    if(current.empty()) {
      // No more open nodes
      return Array<Vector2i>{};
    }

    if(current.key() == parameters.target) {
      Array<Vector2i> path;
      Vector2i iter = current.key();
      while(auto* node = _nodes.find(iter)) {
        if(iter == parameters.start) {
          break;
        } else {
          path.push_front(iter);
          iter = node->parent;
        }
      }
      return path;
    }

    _nodes[current.key()].status = NodeStatus::Closed;

    Array<Vector2i> neighbors = get_neighbors(current.key());
    for(const Vector2i& neighbor : neighbors) {
      Node& neighbor_node = _nodes[neighbor];
      if(neighbor_node.is_obstacle) {
        continue; // Ignore obstacles
      }
      if(neighbor_node.status != NodeStatus::Closed) {
        const float tentative_g_score = current.value().g_score + get_cost(current.key(), neighbor);
        if(neighbor_node.status == NodeStatus::New || tentative_g_score < neighbor_node.g_score) {
          neighbor_node.parent = current.key();
          neighbor_node.g_score = tentative_g_score;
          neighbor_node.f_score = tentative_g_score + get_estimated_cost(neighbor, parameters.target);
          neighbor_node.status = NodeStatus::Open;
        }
      }
    }
  }

  return Array<Vector2i>{};
}
Array<Vector2i> Grid2d::find_zone(const Parameters& parameters) {
  { // Reset node statuses
    for(auto& node : _nodes) {
      node.value().status = NodeStatus::New;
    }
  }

  { // Setup start node
    Node& start_node = _nodes[parameters.start];
    start_node.g_score = 0.f;
    start_node.status = NodeStatus::Open;
  }

  while(true) {
    Table<Vector2i, Node>::Slot current(0, Vector2i());
    for(const auto& node : _nodes) {
      if(node.value().status == NodeStatus::Open) {
        current = node;
        break;
      }
    }

    if(current.empty()) {
      // No more open nodes
      Array<Vector2i> zone;
      for(const auto& node : _nodes) {
        if(node.value().status != NodeStatus::New &&
          node.value().g_score <= parameters.max_dist) {
          zone.push(node.key());
        }
      }
      return zone;
    }

    _nodes[current.key()].status = NodeStatus::Closed;

    if(_nodes[current.key()].g_score > parameters.max_dist) {
      continue;
    }

    Array<Vector2i> neighbors = get_neighbors(current.key());
    for(const Vector2i& neighbor : neighbors) {
      Node& neighbor_node = _nodes[neighbor];
      if(neighbor_node.is_obstacle) {
        continue; // Ignore obstacles
      }
      const float tentative_g_score = current.value().g_score + get_cost(current.key(), neighbor);
      if(neighbor_node.status == NodeStatus::New || tentative_g_score < neighbor_node.g_score) {
        neighbor_node.g_score = tentative_g_score;
        neighbor_node.status = NodeStatus::Open;
      }
    }
  }
}

static Var translate_output(const Array<Vector2i>& points) {
  Ref<Array<Var>> float_points = ref<Array<Var>>(points.size());
  for(uintptr_t i = 0; i < points.size(); i++) {
    (*float_points)[i] = Vector2f(points[i]);
  }
  return float_points;
}

static Vector2i translate_vec2i(const Var& v) {
  Vector2i vec2i;
  if(const Vector2f* vec2f = v.try_as<Vector2f>()) {
    vec2i = *vec2f;
  } else {
    vec2i = Vector2i(0, 0);
  }
  return vec2i;
}

static Grid2d::Parameters fill_parameters(ScriptContext& c) {
  Grid2d::Parameters parameters;
  if(Ref<Table<Var, Var>>* table = c.param(0).try_as<Ref<Table<Var, Var>>>()) {
    if(const Var* start_var = (*table)->find(Symbol("start"))) {
      parameters.start = translate_vec2i(*start_var);
    }

    if(const Var* target_var = (*table)->find(Symbol("target"))) {
      parameters.target = translate_vec2i(*target_var);
    }

    if(const Var* max_dist_var = (*table)->find(Symbol("max_dist"))) {
      parameters.max_dist = max_dist_var->get<float>();
    }
  }

  return parameters;
}

void grid2d_module_init() {
  register_script_function("grid2d_create", [](ScriptContext& c) { c.return_value() = ref<Grid2d>(); });
  L_SCRIPT_REF_METHOD(Grid2d, "set_edge", 3, set_edge(translate_vec2i(c.param(0)), translate_vec2i(c.param(1)), c.param(2)));
  L_SCRIPT_REF_METHOD(Grid2d, "set_obstacle", 2, set_obstacle(translate_vec2i(c.param(0)), c.param(1)));
  L_SCRIPT_REF_WRAPPED_RETURN_METHOD(Grid2d, "find_path", 1, translate_output, find_path(fill_parameters(c)));
  L_SCRIPT_REF_WRAPPED_RETURN_METHOD(Grid2d, "find_zone", 1, translate_output, find_zone(fill_parameters(c)));
}
