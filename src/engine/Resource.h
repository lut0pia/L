#pragma once

#include <atomic>

#include "../container/Buffer.h"
#include "../container/Table.h"
#include "../dynamic/Type.h"
#include "../math/math.h"
#include "../text/String.h"
#include "../text/Symbol.h"
#include "../time/Date.h"

namespace L {
  enum class ResourceState : uint32_t {
    Unloaded,
    Loading,
    Loaded,
    Failed,
  };

  struct ResourceSlot {
    Table<Symbol, Symbol> parameters;
    Symbol type, id, path, ext;
    Buffer source_buffer;

    Array<String> source_files;
    Date mtime;

    std::atomic<ResourceState> state = ResourceState::Unloaded;
    void (*load_function)(ResourceSlot&);
    void* value = nullptr;

    ResourceSlot(const Symbol& type, const char* url);

    Symbol parameter(const Symbol& key) const;
    bool parameter(const Symbol& key, Symbol& value) const;
    bool parameter(const Symbol& key, uint32_t& value) const;
    bool parameter(const Symbol& key, float& value) const;

    void load();
    bool flush();

    Buffer read_source_file();
    Buffer read_archive();
    void write_archive(const void* data, size_t size);

#if !L_RLS
    void read(Stream&);
    void write(Stream&) const;
    Buffer read_archive_dev();
    void write_archive_dev(const void* data, size_t size);
#endif

    bool is_out_of_date() const;

    static Symbol make_typed_id(const Symbol& type, const char* url);
    static ResourceSlot* find(const Symbol& type, const char* url);
    static void set_program_mtime(Date mtime);
    static void update();
#if !L_RLS
    static Array<ResourceSlot*> slots();
#endif
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
    inline const ResourceSlot* slot() const { return _slot; }
    inline bool is_set() const { return _slot != nullptr; }
    inline bool is_loaded() const {
      if(_slot) {
        _slot->load();
        return _slot->state == ResourceState::Loaded && _slot->value;
      } else
        return false;
    }
    inline bool operator==(const Resource& other) const { return slot() == other.slot(); }
    inline bool operator!=(const Resource& other) const { return !operator==(other); }
    inline const T* try_load() const {
      if(_slot) {
        _slot->load();
        return (T*)_slot->value;
      } else {
        return nullptr;
      }
    }
    inline const T* force_load() const {
      if(_slot) {
        _slot->flush();
        return (T*)_slot->value;
      } else {
        return nullptr;
      }
    }

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
      hash_combine(h, v.slot() ? v.slot()->state.load() : ResourceState::Unloaded);
      return h;
    }
  };
}
