#pragma once

#include "macros.h"
#include "containers/Table.h"
#include "containers/Ref.h"
#include "text/String.h"
#include "system/File.h"
#include "stream/CFileStream.h"

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

    virtual Ref<T> from(const File& file) {
      CFileStream fs(file.path(), "rb");
      if(auto wtr = from(fs))
        return wtr;
      else {
        const size_t fileSize(fs.size());
        byte* buffer(Memory::allocType<byte>(fileSize));
        fs.read(buffer, fileSize);
        auto r(from(buffer, fileSize));
        Memory::free(buffer, fileSize);
        return r;
      }
    }
    virtual Ref<T> from(Stream& is) {
      return nullptr;
    }
    virtual Ref<T> from(const byte* data, size_t size) {
      return nullptr;
    }

    virtual bool to(const T& v, const File& file) {
      CFileStream stream(file.path(), "wb");
      return to(v, stream);
    }
    virtual bool to(const T& v, Stream& os) {
      return false;
    }
    virtual bool to(const T& v, byte* data, size_t size) {
      return false;
    }

    static Ref<T> from_file(const char* path) {
      if(auto interfaces = _instances.find(file_ext(path)))
        for(auto interface : *interfaces)
          if(Ref<T> object = interface->from(File(path)))
            return object;
      return nullptr;
    }
    static void to_file(const T& v, const char* path) {
      if(auto interfaces = _instances.find(file_ext(path)))
        for(auto interface : *interfaces)
          if(interface->to(File(path)))
            return
    }
    static const char* file_ext(const char* path) {
      const char* ext(strrchr(path, '.'));
      return ext ? ext+1 : path;
    }
  };
  template <class T> Table<const char*, Array<Interface<T>*>> Interface<T>::_instances;
}
