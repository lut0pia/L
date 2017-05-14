#pragma once

#include "macros.h"
#include "containers/Table.h"
#include "containers/Ref.h"
#include "text/String.h"
#include "system/File.h"
#include "streams/CFileStream.h"

namespace L {
  template <class T>
  class Interface {
  private:
    static Table<const char*, Interface*> _instances;

  protected:
    Interface(const char* format) {
      subscribe(format);
    }
    void subscribe(const char* format) {
      _instances[format] = this;
    }

  public:
    virtual Ref<T> from(const File& file) {
      CFileStream fs(file.path(), "rb");
      if(auto wtr = from(fs))
        return wtr;
      else {
        const size_t fileSize(fs.size());
        byte* buffer(Memory::allocType<byte>(fileSize));
        fs.read(buffer, fileSize);
        return from(buffer, fileSize);
      }
    }
    virtual Ref<T> from(const char* str) {
      tmpfile.rewind();
      tmpfile << str;
      tmpfile.rewind();
      return from(tmpfile);
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
    virtual bool to(const T& v, String& str) {
      return false;
    }
    virtual bool to(const T& v, Stream& os) {
      return false;
    }
    virtual bool to(const T& v, byte* data, size_t size) {
      return false;
    }

    static Interface& in(const char* format) {
      if(auto found = _instances.find(format)) return **found;
      else L_ERRORF("Unhandled format %s", format);
    }
    static Ref<T> fromFile(const String& path) {
      File file(path);
      return in(file.ext().toLower()).from(file);
    }
    static void toFile(const T& v, const String& path) {
      File file(path);
      in(file.ext().toLower()).to(v, file);
    }
  };
  template <class T> Table<const char*, Interface<T>*> Interface<T>::_instances;
}
