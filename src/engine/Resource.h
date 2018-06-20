#pragma once

#include "../container/Array.h"
#include "../container/Pool.h"
#include "../container/Ref.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../dynamic/Type.h"
#include "../parallelism/Lock.h"
#include "../parallelism/TaskSystem.h"
#include "../text/Symbol.h"
#include "../system/File.h"

namespace L {
  class Buffer;
  struct ResourceSlotGeneric {
    Symbol id, path;
    Date mtime;
    bool persistent : 1;
    enum : uint32_t {
      Unloaded, Loading, Loaded,
    } state;

    inline ResourceSlotGeneric(const char* url) : id(url), path(url, min<size_t>(strlen(url), strchr(url, '?')-url)),
      mtime(Date::now()), persistent(false), state(Unloaded) {}
    Symbol parameter(const char* key);
    Buffer read_source_file();
  };
  template <class T>
  struct ResourceSlot : public ResourceSlotGeneric {
    using ResourceSlotGeneric::ResourceSlotGeneric;
    Ref<T> value;
    void load() {
      if(state==Unloaded && cas((uint32_t*)&state, Unloaded, Loading)==Unloaded) {
        TaskSystem::push([](void* p) {
          ResourceSlot<T>& slot(*(ResourceSlot<T>*)p);
          L_SCOPE_MARKERF("load_resource<%s>(%s)", (const char*)type_name<T>(), slot.id);
          load_resource(slot);
          post_load_resource(slot);
          slot.state = Loaded;
        }, this, uint32_t(-1), TaskSystem::NoParent);
      }
    }
  };

  template <class T>
  class Resource {
  public:
    typedef ResourceSlot<T> Slot;
    typedef void (*Loader)(Slot&);
  protected:
    static Table<const char*, Loader> _loaders;
    static Pool<Slot> _pool;
    static Array<Slot*> _slots;
    static Table<Symbol, Slot*> _table;
    Slot* _slot;

  public:
    constexpr Resource() : _slot(nullptr) {}
    inline Resource(const String& url) : Resource((const char*)url) {}
    Resource(const char* url) {
      static Lock lock;
      L_SCOPED_LOCK(lock);

      const Symbol id(url);
      if(Slot** found = _table.find(id))
        _slot = *found;
      else {
        _slot = new(_pool.allocate())Slot(url);
        _table[id] = _slot;
        _slots.push(_slot);
      }
    }
    inline T& operator*() { flush(); return *_slot->value; }
    inline const T& operator*() const { flush(); return *_slot->value; }
    inline T* operator->() { flush(); return _slot->value; }
    inline const T* operator->() const { flush(); return _slot->value; }
    inline const Ref<T>& ref() { flush(); return _slot->value; }
    inline bool is_set() const { return _slot!=nullptr; }
    inline operator bool() const {
      if(_slot) {
        _slot->load();
        return _slot->state == ResourceSlotGeneric::Loaded && _slot->value;
      } else return false;
    }
    inline void flush() const {
      if(_slot && _slot->state != ResourceSlotGeneric::Loaded) {
        if(_slot->state == ResourceSlotGeneric::Unloaded)
          _slot->load();
        L_SCOPE_MARKER("Resource flush");
        TaskSystem::yield_until([](void* data) {
          return ((Slot*)data)->state == ResourceSlotGeneric::Loaded;
        }, _slot);
      }
    }

    friend Stream& operator<(Stream& s, const Resource& v) { return s < (v._slot ? v._slot->id : Symbol("null")); }
    friend Stream& operator>(Stream& s, Resource& v) {
      Symbol id;
      s > id;
      v = id!=Symbol("null") ? Resource(id) : Resource();
      return s;
    }

    static void update() {
      if(!_slots.empty()) {
        L_SCOPE_MARKERF("Resource<%s>::update()", (const char*)type_name<T>());
        static uintptr_t index(0);
        Slot& slot(*_slots[index%_slots.size()]);
        if(!slot.persistent // Persistent resources don't hot reload
           && slot.state == ResourceSlotGeneric::Loaded) { // No reload if it hasn't been loaded in the first place
          Date file_mtime;
          if(File::mtime(slot.path, file_mtime) && slot.mtime<file_mtime) {
            slot.state = ResourceSlotGeneric::Unloaded;
            slot.mtime = Date::now();
          }
        }
        index++;
      }
    }
    static void add_loader(const char* ext, Loader loader) {
      _loaders[ext] = loader;
    }
    static void load(Slot& slot) {
      const char* ext(strrchr(slot.path, '.'));
      ext = ext ? ext + 1 : (const char*)slot.path;
      if(Loader* loader = _loaders.find(ext)) {
        (*loader)(slot);
      } else warning("Unable to load resource with extension: %s", ext);
    }
  };

  template <class T> void load_resource(ResourceSlot<T>& slot) { Resource<T>::load(slot); }
  template <class T> void post_load_resource(ResourceSlot<T>& slot) { }

  template <class T> Table<const char*, typename Resource<T>::Loader> Resource<T>::_loaders;
  template <class T> Pool<ResourceSlot<T>> Resource<T>::_pool;
  template <class T> Array<ResourceSlot<T>*> Resource<T>::_slots;
  template <class T> Table<Symbol, ResourceSlot<T>*> Resource<T>::_table;
}
