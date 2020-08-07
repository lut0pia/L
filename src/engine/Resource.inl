#pragma once

#include "Resource.h"
#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../stream/BufferStream.h"
#include "../stream/StringStream.h"
#include "../text/compression.h"

namespace L {
  template <class T> inline void resource_write_dev(Stream&, const T&) {}
  template <class T> inline void resource_read_dev(Stream&, T&) {}
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

#if !L_RLS
      if(Buffer dev_buffer = slot.read_archive_dev()) { // Look in the dev archive for that resource
        BufferStream dev_stream((char*)dev_buffer.data(), dev_buffer.size());
        {
          L_SCOPE_MARKER("Resource serialize dev");
          slot.read(dev_stream);
          resource_read_dev(dev_stream, intermediate);
        }
      }
#endif

      if(Buffer compressed_buffer = slot.read_archive()) { // Look in the archive for that resource
        Buffer buffer;
        {
          L_SCOPE_MARKER("Resource decompress");
          buffer = lz_decompress(compressed_buffer.data(), compressed_buffer.size());
        }
        {
          L_SCOPE_MARKER("Resource unserialize");
          BufferStream stream((char*)buffer.data(), buffer.size());
          resource_read(stream, intermediate);
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
            resource_write(uncompressed_stream, intermediate);
          }
#if !L_RLS
          StringStream dev_stream;
          {
            L_SCOPE_MARKER("Resource serialize dev");
            slot.write(dev_stream);
            resource_write_dev(dev_stream, intermediate);
          }
          slot.write_archive_dev(dev_stream.string().begin(), dev_stream.string().size());
#endif
          store(slot, intermediate);
          {
            L_SCOPE_MARKER("Resource compress");
            lz_compress(uncompressed_stream.string().begin(), uncompressed_stream.string().size(), compressed_stream);
          }
          slot.write_archive(compressed_stream.string().begin(), compressed_stream.string().size());
          return true;
        } else {
          slot.mtime = Date::now();
          slot.state = ResourceSlot::Failed;
          warning("Unable to load %s from: %s", slot.type, slot.id);
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
      slot.value = Memory::new_type<T>(static_cast<typename T::Intermediate&&>(intermediate));
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