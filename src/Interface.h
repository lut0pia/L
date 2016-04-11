#ifndef DEF_L_Interface
#define DEF_L_Interface

#include "macros.h"
#include "containers/Map.h"
#include "String.h"
#include "containers/Array.h"
#include "system/File.h"
#include "streams/FileStream.h"

namespace L {
  template <class T>
  class Interface {
    private:
      static Map<String,Interface*> instance;

    protected:
      Interface(const char* format) {
        subscribe(format);
      }
      void subscribe(const char* format) {
        instance[format] = this;
      }

    public:
      virtual bool from(T& v, const File& file) {
        FileStream fs(file.path(),"rb");
        return from(v,fs);
      }
      virtual bool from(T& v, const char* str) {
        tmpfile.rewind();
        tmpfile << str;
        tmpfile.rewind();
        return from(v,tmpfile);
      }
      virtual bool from(T& v, Stream& is) {
        return false;
      }
      virtual bool from(T& v, const Array<byte>& bytes) {
        return false;
      }

      virtual bool to(const T& v, const File& file) {
        return to(v,FileStream(file.path(),"wb"));
      }
      virtual bool to(const T& v, String& str) {
        return false;
      }
      virtual bool to(const T& v, Stream& os) {
        return false;
      }
      virtual bool to(const T& v, Array<byte>& bytes) {
        tmpfile.rewind();
        if(!to(v,tmpfile)) // Write in stream
          return false;
        bytes.size(tmpfile.tell());
        tmpfile.rewind();
        tmpfile.read(&bytes[0],bytes.size());
        tmpfile.rewind();
        return true;
      }

      static Interface& in(const String& format) {
        if(instance.has(format)) return *instance[format];
        else L_ERROR("Unhandled format "+format);
      }
      static void fromFile(T& v, const String& path) {
        in(path.explode('.').back().toLower()).from(v,File(path));
      }
      static void toFile(const T& v, const String& path) {
        in(path.explode('.').back().toLower()).to(v,File(path));
      }
  };
  template <class T> Map<String,Interface<T>*> Interface<T>::instance;
}

#endif

