#pragma once

#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../dynamic/Type.h"
#include "../math/math.h"
#include "../parallelism/TaskSystem.h"
#include "../system/intrinsics.h"
#include "../text/Symbol.h"
#include "../time/Date.h"

namespace L {
  class Buffer;
  struct ResourceSlotGeneric {
    Symbol id, path;
    Date mtime;
    bool persistent : 1;
    enum : uint32_t {
      Unloaded, Loading, Loaded,
    } state;
    void* value;

    inline ResourceSlotGeneric(const char* url) : id(url), path(url, min<size_t>(strlen(url), strchr(url, '?')-url)),
      mtime(Date::now()), persistent(false), state(Unloaded), value(nullptr) {}
    Symbol parameter(const char* key);
    Buffer read_source_file();
    static ResourceSlotGeneric* find(const char* url);
    static void update();
  };
  template <class T>
  struct ResourceSlot : public ResourceSlotGeneric {
    using ResourceSlotGeneric::ResourceSlotGeneric;
    void load() {
      if(state==Unloaded && cas((uint32_t*)&state, Unloaded, Loading)==Unloaded) {
        TaskSystem::push([](void* p) {
          ResourceSlot<T>& slot(*(ResourceSlot<T>*)p);
          L_SCOPE_MARKERF("load_resource<%s>(%s)", (const char*)type_name<T>(), slot.id);
          if(slot.value) {
            Memory::delete_type<T>((T*)slot.value);
            slot.value = nullptr;
          }
          load_resource(slot);
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
    Slot* _slot;

  public:
    constexpr Resource() : _slot(nullptr) {}
    inline Resource(const String& url) : Resource((const char*)url) {}
    inline Resource(const char* url) : _slot((Slot*)ResourceSlotGeneric::find(url)) {}
    inline T& operator*() { flush(); return *(T*)_slot->value; }
    inline const T& operator*() const { flush(); return *(T*)_slot->value; }
    inline T* operator->() { flush(); return (T*)_slot->value; }
    inline const T* operator->() const { flush(); return (T*)_slot->value; }
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

  template <class T> Table<const char*, typename Resource<T>::Loader> Resource<T>::_loaders;
}
