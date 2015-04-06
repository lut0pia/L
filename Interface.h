#ifndef DEF_L_Interface
#define DEF_L_Interface

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include "macros.h"
#include "Exception.h"
#include "stl/Map.h"
#include "stl/String.h"
#include "stl/Vector.h"
#include "system/File.h"

namespace L {
  template <class T>
  class Interface {
    private:
      static Map<String,Interface*> instance;

    protected:
      Interface(const String& format) {
        subscribe(format);
      }
      void subscribe(const String& format) {
        instance[format] = this;
      }

    public:
      virtual bool from(T& v, const File& file) {
        std::ifstream is(file.path().c_str(),std::ios::binary);
        return from(v,is);
      }
      virtual bool from(T& v, const String& str) {
        return false;
      }
      virtual bool from(T& v, std::istream& is) {
        return false;
      }
      virtual bool from(T& v, const Vector<byte>& bytes) {
        return false;
      }

      virtual bool to(const T& v, const File& file) {
        std::ofstream os(file.path().c_str(),std::ios::binary);
        return to(v,os);
      }
      virtual bool to(const T& v, String& str) {
        return false;
      }
      virtual bool to(const T& v, std::ostream& os) {
        return false;
      }
      virtual bool to(const T& v, Vector<byte>& bytes) {
        std::stringbuf sb;
        std::iostream stream(&sb); // Create stream
        if(!to(v,stream)) // Write in stream
          return false;
        // Copy data in vector
        bytes.resize(1);
        stream.read((char*)&bytes.front(),1);
        if(stream.gcount()) {
          do {
            bytes.resize(bytes.size()*2);
            stream.read((char*)&bytes[bytes.size()/2],bytes.size()/2);
          } while(stream.gcount()==bytes.size()/2);
          //bytes.resize(bytes.size()-((bytes.size()/2)-stream.gcount()));
          bytes.resize((2*stream.gcount()+bytes.size())/2);
        } else bytes.clear();
        return true;
      }

      static Interface& in(const String& format) {
        if(instance.has(format)) return *instance[format];
        else throw Exception("Unhandled format "+format);
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

