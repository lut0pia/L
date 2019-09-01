#pragma once

#include "Resource.h"
#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../stream/BufferStream.h"
#include "../stream/StringStream.h"
#include "../text/compression.h"

namespace L {
  template <class T> void store_intermediate(ResourceSlot& slot, T*) {
    slot.store_source_file_to_archive();
  }
  template <class T> void store_intermediate(ResourceSlot& slot, const T& intermediate) {
    StringStream uncompressed_stream, compressed_stream;
    {
      L_SCOPE_MARKER("Resource serialize");
      uncompressed_stream <= intermediate;
    }
    {
      L_SCOPE_MARKER("Resource compress");
      lz_compress(uncompressed_stream.string().begin(), uncompressed_stream.string().size(), compressed_stream);
    }
    slot.write_archive(compressed_stream.string().begin(), compressed_stream.string().size());
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
      if(Buffer compressed_buffer = slot.read_archive()) { // Look in the archive for that resource
        Buffer buffer;
        {
          L_SCOPE_MARKER("Resource decompress");
          buffer = lz_decompress(compressed_buffer.data(), compressed_buffer.size());
        }
        {
          L_SCOPE_MARKER("Resource unserialize");
          BufferStream stream((char*)buffer.data(), buffer.size());
          stream >= intermediate;
        }
        return true;
      } else { // Try to load it from source
        if(load_internal(slot, intermediate)) {
          store_intermediate(slot, intermediate);
          return true;
        } else {
          warning("Unable to load resource: %s", slot.id);
        }
        return false;
      }
    }
    static bool load_internal(ResourceSlot& slot, typename T::Intermediate& intermediate) {
      if(Loader* loader = _loaders.find(slot.ext)) {
        return (*loader)(slot, intermediate);
      } else {
        warning("Unable to load resource with extension: %s", slot.ext);
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
      typename T::Intermediate* intermediate(Memory::new_type<typename T::Intermediate>());
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