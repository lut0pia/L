#pragma once

#include "../containers/Array.h"
#include "../containers/Ref.h"
#include "../containers/Table.h"
#include "../font/Font.h"
#include "../gl/Texture.h"
#include "../Interface.h"
#include "../text/Symbol.h"
#include "../script/Context.h"

namespace L {
  template <class T>
  class Resource {
  protected:
    struct Slot {
      Ref<T> value;
      Symbol id;
    };
    static Array<Slot> _array;
    static Table<Symbol, intptr_t> _table;
    intptr_t _index;

    inline Slot& slot() { return _array[_index]; }
    inline const Slot& slot() const { return _array[_index]; }
  public:
    constexpr Resource() : _index(-1) {}
    constexpr Resource(intptr_t index) : _index(index) {}
    inline T& operator*() { return *slot().value; }
    inline T* operator->() { return slot().value; }
    inline operator bool() const { return _index>=0 && slot().value; }
    inline const Ref<T>& ref() { return slot().value; }

    friend Stream& operator<(Stream& s, const Resource& v) {
      if(v) s < v.slot().id;
      else s < Symbol("null");
      return s;
    }
    friend Stream& operator>(Stream& s, Resource& v) {
      Symbol id;
      s > id;
      if(id!=Symbol("null"))
        v = Resource::get(id);
      else v = Resource();
      return s;
    }

    static Resource get(const char* id) {
      const Symbol sym(id);
      if(intptr_t* found = _table.find(sym))
        return Resource(*found);

      Resource wtr(_array.size());
      _array.push();
      wtr.slot().value = load_resource<T>(id);
      wtr.slot().id = sym;
      _table[sym] = wtr._index;
      return wtr;
    }
  };

  template <class T> Ref<T> load_resource(const char* fp) { return Interface<T>::from_file(fp); }
  template <> Ref<GL::Texture> load_resource(const char* fp);
  template <> Ref<Script::CodeFunction> load_resource(const char* fp);
  template <> Ref<Font> load_resource(const char* fp);

  template <class T> Array<typename Resource<T>::Slot> Resource<T>::_array;
  template <class T> Table<Symbol, intptr_t> Resource<T>::_table;
}
