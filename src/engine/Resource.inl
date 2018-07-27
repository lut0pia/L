#pragma once

#include "Resource.h"
#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../dynamic/Type.h"
#include "../stream/BufferStream.h"
#include "../stream/StringStream.h"

namespace L {
  template <class T>
  class ResourceLoading {
    typedef bool(*Loader)(ResourceSlot&, typename T::Intermediate&);
    static Table<const char*, Loader> _loaders;
  public:
    static void add_loader(const char* ext, Loader loader) {
      _loaders[ext] = loader;
    }
    static bool load(ResourceSlot& slot, typename T::Intermediate& intermediate) {
      const char* ext(strrchr(slot.path, '.'));
      ext = ext ? ext + 1 : (const char*)slot.path;
      if(Loader* loader = _loaders.find(ext)) {
        return (*loader)(slot, intermediate);
      } else {
        warning("Unable to load resource with extension: %s", ext);
        return false;
      }
    }
  };
  template <class T> Table<const char*, typename ResourceLoading<T>::Loader> ResourceLoading<T>::_loaders;

  template <class T> void Resource<T>::load() const {
    if(_slot->state==ResourceSlot::Unloaded && cas((uint32_t*)&_slot->state, ResourceSlot::Unloaded, ResourceSlot::Loading)==ResourceSlot::Unloaded) {
      TaskSystem::push([](void* p) {
        ResourceSlot& slot(*(ResourceSlot*)p);
        L_SCOPE_MARKERF("load_resource<%s>(%s)", (const char*)type_name<T>(), slot.id);
        if(slot.value) {
          Memory::delete_type<T>((T*)slot.value);
          slot.value = nullptr;
        }
        slot.dependencies.clear();
        typename T::Intermediate intermediate;
        if(Buffer buffer = slot.read_archive()) {
          BufferStream stream((char*)buffer.data(), buffer.size());
          L_SCOPE_MARKER("Resource unserialize");
          slot.unserialize(stream);
          stream >= intermediate;
          resolve_intermediate<T>(slot, intermediate);
          } else {
          if(ResourceLoading<T>::load(slot, intermediate)) {
            store_intermediate(slot, intermediate);
            resolve_intermediate<T>(slot, intermediate);
          }
        }
        slot.mtime = Date::now();
        slot.state = ResourceSlot::Loaded;
      }, _slot, uint32_t(-1), TaskSystem::NoParent);
    }
  }
  template <class T> void Resource<T>::flush() const {
    if(_slot && _slot->state != ResourceSlot::Loaded) {
      L_SCOPE_MARKER("Resource flush");
      load();
      TaskSystem::yield_until([](void* data) {
        return ((ResourceSlot*)data)->state == ResourceSlot::Loaded;
      }, _slot);
    }
  }

  template <class T> void store_intermediate(ResourceSlot& slot, T* intermediate) {
    slot.store_source_file_to_archive();
  }
  template <class T> void store_intermediate(ResourceSlot& slot, const T& intermediate) {
    StringStream stream;
    slot.serialize(stream);
    stream <= intermediate;
    slot.write_archive(stream.string().begin(), stream.string().size());
  }
  template <class T> void resolve_intermediate(ResourceSlot& slot, T* intermediate) { slot.value = intermediate; }
  template <class T, class R> void resolve_intermediate(ResourceSlot& slot, const R& intermediate) { slot.value = Memory::new_type<T>(intermediate); }
}