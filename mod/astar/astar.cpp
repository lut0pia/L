#include <L/src/container/Array.h>
#include <L/src/container/Table.h>
#include <L/src/script/script_binding.h>

using namespace L;

template<class Node> using AStarDistanceFunc = float(const Node&, const Node&, void*);
template<class Node> using AStarNeighborsFunc = Array<Node>(const Node&, void*);

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

template<class Node>
struct AStarParameters {
  Node start, target;
  float max_dist;
  AStarDistanceFunc<Node>* dist_func;
  AStarNeighborsFunc<Node>* ngbr_func;
  Array<Node> output;
  void* user_data;
};

template<class Node>
bool astar_find_path(AStarParameters<Node>& parameters) {
  Table<Node, AStarNode<Node>> nodes;

  { // Setup start node
    AStarNode<Node>& start_node = nodes[parameters.start];
    start_node.g_score = 0.f;
    start_node.f_score = parameters.dist_func(parameters.start, parameters.target, parameters.user_data);
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

    if(current.key() == parameters.target) {
      Node iter = current.key();
      while(auto* node = nodes.find(iter)) {
        if(iter != parameters.start) {
          parameters.output.push_front(iter);
        }
        iter = node->parent;
      }
      return true;
    }

    nodes[current.key()].status = AStarNodeStatus::Closed;

    Array<Node> neighbors = parameters.ngbr_func(current.key(), parameters.user_data);
    for(const Node& neighbor : neighbors) {
      AStarNode<Node>& node = nodes[neighbor];
      if(node.status != AStarNodeStatus::Closed) {
        const float tentative_g_score = current.value().g_score + parameters.dist_func(current.key(), neighbor, parameters.user_data);
        if(node.status == AStarNodeStatus::New || tentative_g_score < node.g_score) {
          node.parent = current.key();
          node.g_score = tentative_g_score;
          node.f_score = tentative_g_score + parameters.dist_func(neighbor, parameters.target, parameters.user_data);
          node.status = AStarNodeStatus::Open;
        }
      }
    }
  }

  return false;
}

template<class Node>
void astar_find_zone(AStarParameters<Node>& parameters) {
  Table<Node, AStarNode<Node>> nodes;

  { // Setup start node
    AStarNode<Node>& start_node = nodes[parameters.start];
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
        if(node.value().g_score <= parameters.max_dist) {
          parameters.output.push(node.key());
        }
      }
      return;
    }

    nodes[current.key()].status = AStarNodeStatus::Closed;

    if(nodes[current.key()].g_score > parameters.max_dist) {
      continue;
    }

    Array<Node> neighbors = parameters.ngbr_func(current.key(), parameters.user_data);
    for(const Node& neighbor : neighbors) {
      AStarNode<Node>& neighbor_node = nodes[neighbor];
      const float tentative_g_score = current.value().g_score + parameters.dist_func(current.key(), neighbor, parameters.user_data);
      if(neighbor_node.status == AStarNodeStatus::New || tentative_g_score < neighbor_node.g_score) {
        neighbor_node.g_score = tentative_g_score;
        neighbor_node.status = AStarNodeStatus::Open;
      }
    }
  }
}

static bool fill_parameters(AStarParameters<Var>& parameters, ScriptContext& c) {
  if(c.param(2).is<Ref<ScriptFunction>>() &&
    c.param(3).is<Ref<ScriptFunction>>()) {
    parameters.start = c.param(0);
    if(c.param(1).is<float>()) {
      parameters.max_dist = c.param(1);
    } else {
      parameters.target = c.param(1);
    }
    parameters.user_data = &c;
    parameters.dist_func = [](const Var& a, const Var& b, void* user_data) {
      ScriptContext* c = (ScriptContext*)user_data;
      Ref<ScriptFunction> func = c->param(2).as<Ref<ScriptFunction>>();
      return c->execute(func, {a, b}).get<float>();
    };
    parameters.ngbr_func = [](const Var& n, void* user_data) {
      ScriptContext* c = (ScriptContext*)user_data;
      const Var result = c->execute(c->param(3).as<Ref<ScriptFunction>>(), &n, 1);
      if(const Ref<Array<Var>>* result_array_ptr = result.try_as<Ref<Array<Var>>>()) {
        return **result_array_ptr;
      }
      return Array<Var>{};
    };

    return true;
  }

  return false;
}

void astar_module_init() {
  register_script_function("astar_find_path",
    [](ScriptContext& c) {
      AStarParameters<Var> parameters;
      if(fill_parameters(parameters, c)) {
        if(astar_find_path(parameters)) {
          c.return_value() = ref<Array<Var>>(parameters.output);
        } else {
          c.return_value() = false;
        }
      }
    });

  register_script_function("astar_find_zone",
    [](ScriptContext& c) {
      AStarParameters<Var> parameters;
      if(fill_parameters(parameters, c)) {
        astar_find_zone(parameters);
        c.return_value() = ref<Array<Var>>(parameters.output);
      }
    });
}
