#ifndef DEF_L_List
#define DEF_L_List

namespace L {
  template <class T>
  class List {
    protected:
      class Node {
        private:
          T _value;
          Node* _next;
        public:
          template <typename... Args>
          Node(Args&&... args) : _value(args...), _next(0) {}
          inline const Node* next() const {return _next;};
          inline const T& value() const {return _value;};
          friend class List;
      };
      Node *_first, *_last;
      size_t _size;
    public:
      List() : _first(0), _last(0), _size(0) {}

      inline const T& front() const {return _first->value();}
      inline const T& back() const {return _last->value();}
      inline const Node* first() const {return _first;}
      inline const Node* last() const {return _last;}
      inline size_t size() const {return _size;}
      inline bool empty() const {return !size();}

      template <typename... Args>
      void push(Args&&... args) {
        Node* node(new Node(args...));
        if(empty()) _first = node;
        else _last->_next = node;
        _last = node;
        _size++;
      }
      template <typename... Args>
      void pushFront(Args&&... args) {
        Node* node(new Node(args...));
        if(empty()) _last = node;
        node->_next = _first;
        _first = node;
        _size++;
      }
      void pop() {
      }
      void popFront() {
        if(_first) {
          Node* old(_first);
          _first = old->_next;
          delete old;
          _size--;
        }
      }
      void clear() {
        Node *current(_first), *next;
        while(current) {
          next = current->_next;
          delete current;
          current = next;
        }
        _first = _last = 0;
      }
  };
}

#endif


