#pragma once

#include "../math/math.h"
#include "../text/String.h"
#include "../text/Symbol.h"
#include "../time/Date.h"

namespace L {
  class Buffer;
  struct ResourceSlot {
    Symbol id, path;
    Date mtime;
    bool persistent : 1;
    enum : uint32_t {
      Unloaded, Loading, Loaded,
    } state;
    void* value;

    inline ResourceSlot(const char* url) : id(url), path(url, min<size_t>(strlen(url), strchr(url, '?')-url)),
      persistent(false), state(Unloaded), value(nullptr) {
    }
    Symbol parameter(const char* key);
    Buffer read_source_file();
    void store_source_file_to_archive();
    Buffer read_archive();
    void write_archive(const void* data, size_t size);
    static ResourceSlot* find(const char* url);
    static void update();
  };

  template <class T>
  class Resource {
  protected:
    ResourceSlot* _slot;

  public:
    constexpr Resource() : _slot(nullptr) {}
    inline Resource(const String& url) : Resource((const char*)url) {}
    inline Resource(const char* url) : _slot(ResourceSlot::find(url)) {}
    inline T& operator*() { flush(); return *(T*)_slot->value; }
    inline const T& operator*() const { flush(); return *(T*)_slot->value; }
    inline T* operator->() { flush(); return (T*)_slot->value; }
    inline const T* operator->() const { flush(); return (T*)_slot->value; }
    inline bool is_set() const { return _slot!=nullptr; }
    inline operator bool() const {
      if(_slot) {
        load();
        return _slot->state == ResourceSlot::Loaded && _slot->value;
      } else return false;
    }
    void load() const;
    void flush() const;

    friend Stream& operator<(Stream& s, const Resource& v) { return s < (v._slot ? v._slot->id : Symbol("null")); }
    friend Stream& operator>(Stream& s, Resource& v) {
      Symbol id;
      s > id;
      v = id!=Symbol("null") ? Resource(id) : Resource();
      return s;
    }
  };
}
