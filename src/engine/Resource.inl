#pragma once

#include "Resource.h"
#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../stream/BufferStream.h"
#include "../stream/StringStream.h"
#include "../text/compression.h"

namespace L {
  template <class T>
  class ResourceLoading {
    typedef bool(*Loader)(ResourceSlot&, typename T::Intermediate&);
    typedef void(*Transformer)(const ResourceSlot&, typename T::Intermediate&);
    static Array<Loader> _loaders;
    static Array<Transformer> _transformers;
  public:
    static void add_loader(Loader loader) {
      _loaders.push(loader);
    }
    static void add_transformer(Transformer transformer) {
      _transformers.push(transformer);
    }
    static bool load(ResourceSlot& slot) {
      if(slot.value) {
        Memory::delete_type<T>((T*)slot.value);
        slot.value = nullptr;
      }

      typename T::Intermediate intermediate {};

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
        store(slot, intermediate);
        return true;
      } else { // Try to load it from source
        if(load_internal(slot, intermediate)) {
          for(Transformer transformer : _transformers) {
            transformer(slot, intermediate);
          }

          StringStream uncompressed_stream, compressed_stream;
          {
            L_SCOPE_MARKER("Resource serialize");
            uncompressed_stream <= intermediate;
          }
          store(slot, intermediate);
          {
            L_SCOPE_MARKER("Resource compress");
            lz_compress(uncompressed_stream.string().begin(), uncompressed_stream.string().size(), compressed_stream);
          }
          slot.write_archive(compressed_stream.string().begin(), compressed_stream.string().size());
          return true;
        } else {
          warning("Unable to load resource: %s", slot.id);
        }
        return false;
      }
    }
    static bool load_internal(ResourceSlot& slot, typename T::Intermediate& intermediate) {
      for(Loader loader : _loaders) {
        if(loader(slot, intermediate)) {
          return true;
        }
      }
      return false;
    }
    static void store(ResourceSlot& slot, typename T::Intermediate& intermediate) {
      slot.value = Memory::new_type<T>(std::move(intermediate));
      slot.mtime = Date::now();
      slot.state = ResourceSlot::Loaded;
    }
  };
  template <class T> Array<typename ResourceLoading<T>::Loader> ResourceLoading<T>::_loaders;
  template <class T> Array<typename ResourceLoading<T>::Transformer> ResourceLoading<T>::_transformers;

  template <class T> void Resource<T>::load_function(ResourceSlot& slot) {
    ResourceLoading<T>::load(slot);
  }
}