#include <L/src/container/Array.h>
#include <L/src/container/Table.h>
#include <L/src/script/script_binding.h>

using namespace L;

enum class AStarNodeStatus : uint8_t {
  New,
  Open,
  Closed,
};

template<class Node>
struct AStarNode {
  float g_score, f_score;
  AStarNodeStatus status = AStarNodeStatus::New;
  Node parent;
};

template<class Node, class Distance, class Neighbors>
bool astar_find_path(const Node& start, const Node& target, Distance dist_func, Neighbors ngbr_func, Array<Node>& path) {
  Table<Node, AStarNode<Node>> nodes;

  { // Setup start node
    AStarNode<Node>& start_node = nodes[start];
    start_node.g_score = 0.f;
    start_node.f_score = dist_func(start, target);
    start_node.status = AStarNodeStatus::Open;
  }

  while(true) {
    Table<Node, AStarNode<Node>>::Slot current(0, Node());
    for(const Table<Node, AStarNode<Node>>::Slot& node : nodes) {
      if(node.value().status == AStarNodeStatus::Open &&
        (current.empty() || node.value().f_score < current.value().f_score)) {
        current = node;
      }
    }

    if(current.empty()) {
      // No more open nodes
      return false;
    }

    if(current.key() == target) {
      Node iter = current.key();
      while(auto* node = nodes.find(iter)) {
        if(iter != start) {
          path.push_front(iter);
        }
        iter = node->parent;
      }
      return true;
    }

    nodes[current.key()].status = AStarNodeStatus::Closed;

    Array<Node> neighbors = ngbr_func(current.key());
    for(const Node& neighbor : neighbors) {
      AStarNode<Node>& node = nodes[neighbor];
      if(node.status != AStarNodeStatus::Closed) {
        const float tentative_g_score = current.value().g_score + dist_func(current.key(), neighbor);
        if(node.status == AStarNodeStatus::New || tentative_g_score < node.g_score) {
          node.parent = current.key();
          node.g_score = tentative_g_score;
          node.f_score = tentative_g_score + dist_func(neighbor, target);
          node.status = AStarNodeStatus::Open;
        }
      }
    }
  }

  return false;
}

template<class Node, class Distance, class Neighbors>
void astar_find_zone(const Node& start, const float max_dist, Distance dist_func, Neighbors ngbr_func, Array<Node>& zone) {
  Table<Node, AStarNode<Node>> nodes;

  { // Setup start node
    AStarNode<Node>& start_node = nodes[start];
    start_node.g_score = 0.f;
    start_node.status = AStarNodeStatus::Open;
  }

  while(true) {
    Table<Node, AStarNode<Node>>::Slot current(0, Node());
    for(const Table<Node, AStarNode<Node>>::Slot& node : nodes) {
      if(node.value().status == AStarNodeStatus::Open) {
        current = node;
        break;
      }
    }

    if(current.empty()) {
      // No more open nodes
      for(const Table<Node, AStarNode<Node>>::Slot& node : nodes) {
        if(node.value().g_score <= max_dist) {
          zone.push(node.key());
        }
      }
      return;
    }

    nodes[current.key()].status = AStarNodeStatus::Closed;

    if(nodes[current.key()].g_score > max_dist) {
      continue;
    }

    Array<Node> neighbors = ngbr_func(current.key());
    for(const Node& neighbor : neighbors) {
      AStarNode<Node>& neighbor_node = nodes[neighbor];
      const float tentative_g_score = current.value().g_score + dist_func(current.key(), neighbor);
      if(neighbor_node.status == AStarNodeStatus::New || tentative_g_score < neighbor_node.g_score) {
        neighbor_node.g_score = tentative_g_score;
        neighbor_node.status = AStarNodeStatus::Open;
      }
    }
  }
}

void astar_module_init() {
  register_script_function("astar_find_path",
    [](ScriptContext& c) {
      Ref<ScriptFunction>* distance_func_ptr = c.param(2).try_as<Ref<ScriptFunction>>();
      Ref<ScriptFunction>* neighbors_func_ptr = c.param(3).try_as<Ref<ScriptFunction>>();
      if(distance_func_ptr && neighbors_func_ptr) {
        Ref<ScriptFunction> distance_func = *distance_func_ptr;
        Ref<ScriptFunction> neighbors_func = *neighbors_func_ptr;
        const auto distance = [&c, distance_func](const Var& a, const Var& b) {
          return c.execute(distance_func, {a, b}).get<float>();
        };
        const auto neighbors = [&c, neighbors_func](const Var& n) {
          const Var result = c.execute(neighbors_func, &n, 1);
          if(const Ref<Array<Var>>* result_array_ptr = result.try_as<Ref<Array<Var>>>()) {
            return **result_array_ptr;
          }
          return Array<Var>{};
        };
        Ref<Array<Var>> path = ref<Array<Var>>();
        const Var start = c.param(0);
        const Var target = c.param(1);
        const bool success = astar_find_path(start, target, distance, neighbors, *path);
        c.return_value() = success ? path : false;
      }
    });

  register_script_function("astar_find_zone",
    [](ScriptContext& c) {
      Ref<ScriptFunction>* distance_func_ptr = c.param(2).try_as<Ref<ScriptFunction>>();
      Ref<ScriptFunction>* neighbors_func_ptr = c.param(3).try_as<Ref<ScriptFunction>>();
      if(distance_func_ptr && neighbors_func_ptr) {
        Ref<ScriptFunction> distance_func = *distance_func_ptr;
        Ref<ScriptFunction> neighbors_func = *neighbors_func_ptr;
        const auto distance = [&c, distance_func](const Var& a, const Var& b) {
          return c.execute(distance_func, {a, b}).get<float>();
        };
        const auto neighbors = [&c, neighbors_func](const Var& n) {
          const Var result = c.execute(neighbors_func, &n, 1);
          if(const Ref<Array<Var>>* result_array_ptr = result.try_as<Ref<Array<Var>>>()) {
            return **result_array_ptr;
          }
          return Array<Var>{};
        };
        Ref<Array<Var>> zone = ref<Array<Var>>();
        const Var start = c.param(0);
        const float max_dist = c.param(1);
        astar_find_zone(start, max_dist, distance, neighbors, *zone);
        c.return_value() = zone;
      }
    });
}
