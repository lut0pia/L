#pragma once

#include "../container/Array.h"
#include "../container/Ref.h"
#include "../container/Table.h"
#include "../font/Font.h"
#include "../gl/Texture.h"
#include "../Interface.h"
#include "../text/Symbol.h"
#include "../time/Date.h"
#include "../script/Context.h"

namespace L {
  struct ResourceSettings {
    bool persistent : 1;
  };
  template <class T>
  class Resource {
  protected:
    struct Slot {
      Ref<T> value;
      Symbol path;
      Date mtime;
      File file;
      ResourceSettings settings;
    };
  protected:
    static Array<Slot> _slots;
    static Table<Symbol, intptr_t> _table;
    intptr_t _index;

    inline Slot& slot() { return _slots[_index]; }
    inline const Slot& slot() const { return _slots[_index]; }
  public:
    constexpr Resource() : _index(-1) {}
    constexpr Resource(intptr_t index) : _index(index) {}
    inline T& operator*() { return *slot().value; }
    inline T* operator->() { return slot().value; }
    inline operator bool() const { return _index>=0 && slot().value; }
    inline const Ref<T>& ref() { return slot().value; }

    friend Stream& operator<(Stream& s, const Resource& v) {
      if(v) s < v.slot().path;
      else s < Symbol("null");
      return s;
    }
    friend Stream& operator>(Stream& s, Resource& v) {
      Symbol path;
      s > path;
      if(path!=Symbol("null"))
        v = Resource::get(path);
      else v = Resource();
      return s;
    }

    static Resource get(const char* path) {
      const Symbol sym(path);
      if(intptr_t* found = _table.find(sym))
        return Resource(*found);

      Resource wtr(_slots.size());
      _slots.push(Slot{
        nullptr, path, Date::now(), path
      });
      Slot& slot(_slots.back());
      slot.value = load_resource<T>(slot.path, slot.settings);
      _table[sym] = wtr._index;
      return wtr;
    }
    static void update() {
      if(!_slots.empty()) {
        static uintptr_t index(0);
        Slot& slot(_slots[index%_slots.size()]);
        if(!slot.settings.persistent) { // Persistent resources don't hot reload
          Date file_mtime;
          if(slot.file.mtime(file_mtime) && slot.mtime<file_mtime) {
            slot.value = load_resource<T>(slot.path, slot.settings);
            slot.mtime = Date::now();
          }
        }
        index++;
      }
    }
  };

  template <class T> Ref<T> load_resource(const char* path, ResourceSettings&) { return Interface<T>::from_file(path); }
  template <> Ref<GL::Texture> load_resource(const char* path, ResourceSettings&);
  template <> Ref<Script::CodeFunction> load_resource(const char* path, ResourceSettings&);
  template <> Ref<Font> load_resource(const char* path, ResourceSettings&);

  template <class T> Array<typename Resource<T>::Slot> Resource<T>::_slots;
  template <class T> Table<Symbol, intptr_t> Resource<T>::_table;
}
