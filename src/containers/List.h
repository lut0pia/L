#pragma once

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
      Node(Args&&... args) : _value(args...),_next(0) {}
      inline const Node* next() const { return _next; };
      inline const T& value() const { return _value; };
      inline T& value() { return _value; };
      friend class List;
    };
    class Iterator{
    private:
      Node* _node;

    public:
      Iterator(Node* n) : _node(n) {}
      inline Iterator& operator++(){ _node = _node->_next; return *this; }
      inline bool operator!=(const Iterator& other) const{ return _node != other._node; }
      inline T* operator->() const{ return (T*)&_node->value(); }
      inline T& operator*() const{ return *(operator->()); }
    };
    Node *_first,*_last;
    size_t _size;
  public:
    constexpr List() : _first(0),_last(0),_size(0) {}

    inline const T& front() const { return _first->value(); }
    inline T& front() { return _first->value(); }
    inline const T& back() const { return _last->value(); }
    inline T& back() { return _last->value(); }
    inline const Node* first() const { return _first; }
    inline const Node* last() const { return _last; }
    inline size_t size() const { return _size; }
    inline bool empty() const { return !size(); }

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
      Node *current(_first),*next;
      while(current) {
        next = current->_next;
        delete current;
        current = next;
      }
      _first = _last = 0;
    }

    inline Iterator begin() const{ return Iterator(_first); }
    inline Iterator end() const{ return Iterator(nullptr); }
  };
}
