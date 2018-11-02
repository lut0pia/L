#pragma once

#include "Resource.h"
#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../stream/BufferStream.h"
#include "../stream/StringStream.h"

namespace L {
  template <class T> void store_intermediate(ResourceSlot& slot, T* intermediate) {
    slot.store_source_file_to_archive();
  }
  template <class T> void store_intermediate(ResourceSlot& slot, const T& intermediate) {
    StringStream stream;
    slot.serialize(stream);
    stream <= intermediate;
    slot.write_archive(stream.string().begin(), stream.string().size());
  }

  template <class T>
  class ResourceLoading {
    typedef bool(*Loader)(ResourceSlot&, typename T::Intermediate&);
    static Table<const char*, Loader> _loaders;
  public:
    static void add_loader(const char* ext, Loader loader) {
      _loaders[ext] = loader;
    }
    static bool load(ResourceSlot& slot, typename T::Intermediate& intermediate) {
      if(Buffer buffer = slot.read_archive()) { // Look in the archive for that resource
        BufferStream stream((char*)buffer.data(), buffer.size());
        L_SCOPE_MARKER("Resource unserialize");
        slot.unserialize(stream);
        stream >= intermediate;
        return slot.flush_all_dependencies();
      } else { // Try to load it from source
        const char* ext(strrchr(slot.path, '.'));
        ext = ext ? ext + 1 : (const char*)slot.path;
        if(Loader* loader = _loaders.find(ext)) {
          if((*loader)(slot, intermediate)) {
            store_intermediate(slot, intermediate);
            return slot.flush_all_dependencies();
          } else {
            warning("Unable to load resource: %s", slot.id);
          }
        } else {
          warning("Unable to load resource with extension: %s", ext);
        }
        return false;
      }

    }
  };
  template <class T> Table<const char*, typename ResourceLoading<T>::Loader> ResourceLoading<T>::_loaders;

  // General case where the intermediate type can be anything
  template <class T, class Inter>
  struct ResourceLoader {
    static void load(ResourceSlot& slot) {
      typename T::Intermediate intermediate {};
      if(ResourceLoading<T>::load(slot, intermediate)) {
        slot.value = Memory::new_type<T>(intermediate);
      }
    }
  };

  // Specific case where the intermediate type is the actual type
  template <class T>
  struct ResourceLoader<T, T> {
    static void load(ResourceSlot& slot) {
      typename T::Intermediate* intermediate(Memory::new_type<T::Intermediate>());
      if(ResourceLoading<T>::load(slot, *intermediate)) {
        slot.value = intermediate;
      } else {
        Memory::delete_type(intermediate);
      }
    }
  };

  // Specific case where the intermediate type is a pointer to the type
  // (legacy codepath, should be removed at some point)
  template <class T>
  struct ResourceLoader<T, T*> {
    static void load(ResourceSlot& slot) {
      T* intermediate(nullptr);
      if(ResourceLoading<T>::load(slot, intermediate)) {
        slot.value = intermediate;
      } else if(intermediate) {
        Memory::delete_type(intermediate);
      }
    }
  };

  template <class T> void Resource<T>::load_function(ResourceSlot& slot) {
    if(slot.value) {
      Memory::delete_type<T>((T*)slot.value);
      slot.value = nullptr;
    }
    ResourceLoader<T, typename T::Intermediate>::load(slot);
  }
}