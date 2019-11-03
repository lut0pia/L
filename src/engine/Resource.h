#pragma once

#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dynamic/Type.h"
#include "../math/math.h"
#include "../text/String.h"
#include "../text/Symbol.h"
#include "../time/Date.h"

namespace L {
  class Buffer;
  struct ResourceSlot {
    Table<Symbol, Symbol> parameters;
    Symbol type, id, path, ext;
    Buffer source_buffer;
    Date mtime;
    bool persistent : 1;
    enum : uint32_t { // 32bits because of atomic operations
      Unloaded, Loading, Loaded,
    } state;
    void(*load_function)(ResourceSlot&);
    void* value;

    ResourceSlot(const Symbol& type, const char* url);

    Symbol parameter(const Symbol& key) const;
    bool parameter(const Symbol& key, Symbol& value) const;
    bool parameter(const Symbol& key, uint32_t& value) const;
    bool parameter(const Symbol& key, float& value) const;

    void load();
    bool flush();

    Buffer read_source_file();
    void store_source_file_to_archive();
    Buffer read_archive();
    void write_archive(const void* data, size_t size);
    static Symbol make_typed_id(const Symbol& type, const char* url);
    static ResourceSlot* find(const Symbol& type, const char* url);
    static void update();
  };

  template <class T>
  class Resource {
  protected:
    ResourceSlot* _slot;

  public:
    constexpr Resource() : _slot(nullptr) {}
    inline Resource(const String& url) : Resource((const char*)url) {}
    inline Resource(const char* url) : _slot(ResourceSlot::find(type_name<T>(), url)) {
      _slot->load_function = load_function;
    }
    inline const T& operator*() const { flush(); return *(T*)_slot->value; }
    inline const T* operator->() const { flush(); return (T*)_slot->value; }
    inline const ResourceSlot* slot() const { return _slot; }
    inline bool is_set() const { return _slot!=nullptr; }
    inline bool is_loaded() const {
      if(_slot) {
        load();
        return _slot->state == ResourceSlot::Loaded && _slot->value;
      } else return false;
    }
    inline operator bool() const { return is_loaded(); }
    inline bool operator==(const Resource& other) { return slot()==other.slot(); }
    inline bool operator!=(const Resource& other) { return !operator==(other); }
    inline void load() const { if(_slot) _slot->load(); }
    inline void flush() const { if(_slot) _slot->flush(); }

    static void load_function(ResourceSlot&);

    friend inline Stream& operator<(Stream& s, const Resource& v) { return s < (v._slot ? v._slot->id : Symbol()); }
    friend inline Stream& operator>(Stream& s, Resource& v) {
      Symbol id;
      s > id;
      v = id ? Resource(id) : Resource();
      return s;
    }
    friend inline Stream& operator<=(Stream& s, const Resource& v) { return s <= (v._slot ? v._slot->id : Symbol()); }
    friend inline Stream& operator>=(Stream& s, Resource& v) {
      Symbol id;
      s >= id;
      v = id ? Resource(id) : Resource();
      return s;
    }

    friend inline uint32_t hash(const Resource& v) {
      uint32_t h = hash(v.slot());
      hash_combine(h, v.slot() ? v.slot()->value : nullptr);
      hash_combine(h, v.slot() ? v.slot()->state : ResourceSlot::Unloaded);
      return h;
    }
  };
}
