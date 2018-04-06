#pragma once

#include "../container/Array.h"
#include "../container/Pool.h"
#include "../container/Ref.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "Interface.h"
#include "../parallelism/Lock.h"
#include "../parallelism/TaskSystem.h"
#include "../text/Symbol.h"
#include "../system/File.h"

namespace L {
  struct ResourceSlotGeneric {
    Symbol id, path;
    Date mtime;
    bool persistent : 1;
    bool loading : 1;

    Symbol parameter(const char* key);
  };
  template <class T>
  struct ResourceSlot : ResourceSlotGeneric {
    Ref<T> value;
  };

  template <class T> void load_resource_async(ResourceSlot<T>& slot) {
    slot.loading = true;
    TaskSystem::push([](void* p) {
      ResourceSlot<T>& slot(*(ResourceSlot<T>*)p);
      L_SCOPE_MARKERF("load_resource(%s)", slot.id);
      load_resource(slot);
      slot.loading = false;
    }, &slot, TaskSystem::NoParent);
  }
  template <class T> void load_resource(ResourceSlot<T>& slot) {
    slot.value = Interface<T>::from_path(slot.path);
    post_load_resource(slot);
  }
  template <class T> void post_load_resource(ResourceSlot<T>& slot) { }

  template <class T>
  class Resource {
    typedef ResourceSlot<T> Slot;
  protected:
    static Pool<Slot> _pool;
    static Array<Slot*> _slots;
    static Table<Symbol, Slot*> _table;
    Slot* _slot;

  public:
    constexpr Resource() : _slot(nullptr) {}
    constexpr Resource(Slot* slot) : _slot(slot) {}
    inline T& operator*() { return *_slot->value; }
    inline const T& operator*() const { return *_slot->value; }
    inline T* operator->() { return _slot->value; }
    inline const T* operator->() const { return _slot->value; }
    inline operator bool() const { return _slot && !_slot->loading && _slot->value; }
    inline const Ref<T>& ref() { return _slot->value; }

    friend Stream& operator<(Stream& s, const Resource& v) {
      if(v) s < v._slot->path;
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

    static Resource get(const char* url, bool synchronous = false) {
      static Lock lock;
      L_SCOPED_LOCK(lock);

      const Symbol id(url);
      if(Slot** found = _table.find(id))
        return Resource(*found);

      Resource wtr(new(_pool.construct())Slot);
      Slot& slot(*wtr._slot);
      slot.id = id;
      slot.path = Symbol(url, min<size_t>(strlen(url), strchr(url, '?')-url)); // Remove parameter part
      slot.mtime = Date::now();
      if(synchronous) load_resource<T>(slot);
      else load_resource_async<T>(slot);
      _table[id] = wtr._slot;
      _slots.push(wtr._slot);
      return wtr;
    }
    static void update() {
      if(!_slots.empty()) {
        static uintptr_t index(0);
        Slot& slot(*_slots[index%_slots.size()]);
        if(!slot.persistent) { // Persistent resources don't hot reload
          Date file_mtime;
          if(File::mtime(slot.path, file_mtime) && slot.mtime<file_mtime) {
            load_resource_async<T>(slot);
            slot.mtime = Date::now();
          }
        }
        index++;
      }
    }
  };

  template <class T> Pool<ResourceSlot<T>> Resource<T>::_pool;
  template <class T> Array<ResourceSlot<T>*> Resource<T>::_slots;
  template <class T> Table<Symbol, ResourceSlot<T>*> Resource<T>::_table;
}
