#include "Graph.h"

using namespace L;

void Graph::add(Arc arc, double v) {
  arcs[arc] = v;
  vertices.insert(arc.head);
  vertices.insert(arc.tail);
}
void Graph::add(Edge edge, double v) {
  arcs[Arc(edge.a,edge.b)] = arcs[Arc(edge.b,edge.a)] = v;
  vertices.insert(edge.a);
  vertices.insert(edge.b);
}

bool Graph::has(size_t vertex) {
  return vertices.has(vertex);
}
bool Graph::has(Arc arc) {
  return arcs.has(arc);
}
bool Graph::has(Edge edge) {
  return arcs.has(Arc(edge.a,edge.b)) && arcs.has(Arc(edge.b,edge.a));
}
