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
      return from(fs);
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
    virtual Ref<T> from(const Array<byte>& bytes) {
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
    virtual bool to(const T& v, Array<byte>& bytes) {
      tmpfile.rewind();
      if(!to(v, tmpfile)) // Write in stream
        return false;
      bytes.size(tmpfile.tell());
      tmpfile.rewind();
      tmpfile.read(&bytes[0], bytes.size());
      tmpfile.rewind();
      return true;
    }

    static Interface& in(const char* format) {
      if(auto found = _instances.find(format)) return **found;
      else L_ERRORF("Unhandled format %s", format);
    }
    static Ref<T> fromFile(const String& path) {
      return in(path.explode('.').back().toLower()).from(File(path));
    }
    static void toFile(const T& v, const String& path) {
      in(path.explode('.').back().toLower()).to(v, File(path));
    }
  };
  template <class T> Table<const char*, Interface<T>*> Interface<T>::_instances;
}
