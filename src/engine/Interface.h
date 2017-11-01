#pragma once

#include "../macros.h"
#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../container/Ref.h"
#include "../text/String.h"
#include "../stream/CFileStream.h"
#include "../stream/BufferStream.h"

namespace L {
  template <class T>
  class Interface {
  private:
    static Table<const char*, Array<Interface*>> _instances;

  public:
    inline Interface(std::initializer_list<const char*> formats) {
      for(auto format : formats)
        _instances[format].push(this);
    }

    virtual Ref<T> from(const char* path) {
      CFileStream fs(path, "rb");
      if(fs) {
        const size_t file_size(fs.size());
        Buffer buffer(file_size);
        fs.read(buffer.data(), file_size);
        return from((const byte*)buffer.data(), file_size);
      } else return nullptr;
    }
    virtual Ref<T> from(Stream& is) {
      return nullptr;
    }
    virtual Ref<T> from(const byte* data, size_t size) {
      BufferStream bs((char*)data, size);
      return from(bs);
    }

    virtual bool to(const T& v, const char* path) {
      CFileStream stream(path, "wb");
      return to(v, stream);
    }
    virtual bool to(const T& v, Stream& os) {
      return false;
    }
    virtual bool to(const T& v, byte* data, size_t size) {
      return false;
    }

    static Ref<T> from_path(const char* path) {
      if(auto interfaces = _instances.find(file_ext(path)))
        for(auto interface : *interfaces)
          if(Ref<T> object = interface->from(path))
            return object;
      return nullptr;
    }
    static void to_path(const T& v, const char* path) {
      if(auto interfaces = _instances.find(file_ext(path)))
        for(auto interface : *interfaces)
          if(interface->to(path))
            return;
    }
    static const char* file_ext(const char* path) {
      const char* ext(strrchr(path, '.'));
      return ext ? ext+1 : path;
    }
  };
  template <class T> Table<const char*, Array<Interface<T>*>> Interface<T>::_instances;
}
