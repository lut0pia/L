#pragma once

#include "../container/Array.h"
#include "../text/String.h"

namespace L {
  class FileWatch {
  protected:
    String _root;
    Array<String> _changes;
    void* _pimpl;

  public:
    FileWatch(const char* root_path);
    ~FileWatch();
    void update();

    inline const Array<String>& get_changes() const { return _changes; }
    inline void clear() { _changes.clear(); }
  };
}
