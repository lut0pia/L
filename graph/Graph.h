#ifndef DEF_L_Graph
#define DEF_L_Graph

#include "../stl/Map.h"
#include "../containers/Set.h"

/*
    Arcs are directed, Edges aren't
*/

namespace L {
  class Graph {
      class Arc {
        public:
          size_t head, tail;
          Arc(size_t head, size_t tail) : head(head), tail(tail) {}
          bool operator<(const Arc& other) const {
            if(head==other.head)
              return tail < other.tail;
            else
              return head < other.head;
          }
      };
      class Edge {
        public:
          size_t a, b;
          Edge(size_t a, size_t b) : a(a), b(b) {}
      };
    protected:
      Map<Arc,double> arcs;
      Set<size_t> vertices;
    public:
      void add(Arc,double=1);
      void add(Edge,double=1);

      bool has(size_t);
      bool has(Arc);
      bool has(Edge);

      size_t size();
  };
}

#endif




