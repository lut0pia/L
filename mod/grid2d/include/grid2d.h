#pragma once

#include <L/src/container/Array.h>
#include <L/src/container/KeyValue.h>
#include <L/src/container/Table.h>
#include <L/src/math/Vector.h>

class Grid2d {
public:
  struct Parameters {
    L::Vector2i start, target;
    float max_dist = 0.f;
    bool ignore_obstacles = false;
  };

protected:
  enum class NodeStatus : uint8_t {
    New,
    Open,
    Closed,
  };

  struct Node {
    L::Table<L::Vector2i, float> neighbors;

    L::Vector2i parent;
    float g_score, f_score;
    NodeStatus status = NodeStatus::New;
    bool is_obstacle = false;
  };

  L::Table<L::Vector2i, Node> _nodes;

  L::Array<L::Vector2i> get_neighbors(const L::Vector2i&);
  float get_estimated_cost(const L::Vector2i& from, const L::Vector2i& to);
  float get_cost(const L::Vector2i& from, const L::Vector2i& to);
public:
  void set_edge(const L::Vector2i& start, const L::Vector2i& end, float cost);
  void set_obstacle(const L::Vector2i& position, bool is_obstacle);
  L::Array<L::Vector2i> find_path(const Parameters& parameters);
  L::Array<L::Vector2i> find_zone(const Parameters& parameters);
};
