#include "RmlUiFile.h"

#include <L/src/engine/Resource.inl>

using namespace L;

Rml::Core::FileHandle RmlUiFile::Open(const Rml::Core::String& path) {
  File file;
  file.resource = path.c_str();
  if(!_resources.find(file.resource)) {
    _resources.push(file.resource);
  }
  _files[_next_handle] = file;
  return _next_handle++;
}
void RmlUiFile::Close(Rml::Core::FileHandle handle) {
  _files.remove(handle);
}

size_t RmlUiFile::Read(void* dst, size_t size, Rml::Core::FileHandle handle) {
  File& file = _files[handle];
  if(const Buffer* buffer = file.resource.force_load()) {
    const void* data = buffer->data();
    const size_t max_size = min(size, buffer->size() - file.offset);
    memcpy(dst, (uint8_t*)data + file.offset, max_size);
    file.offset += max_size;
    return max_size;
  }
  return 0;
}
bool RmlUiFile::Seek(Rml::Core::FileHandle handle, long offset, int origin) {
  File& file = _files[handle];
  if(const Buffer* buffer = file.resource.force_load()) {
    switch(origin) {
      case SEEK_SET: file.offset = offset; break;
      case SEEK_CUR: file.offset += offset; break;
      case SEEK_END: file.offset = buffer->size() - offset;
      default: return false;
    }
    return true;
  }
  return false;
}
size_t RmlUiFile::Tell(Rml::Core::FileHandle handle) {
  File& file = _files[handle];
  return file.offset;
}

size_t RmlUiFile::Length(Rml::Core::FileHandle handle) {
  File& file = _files[handle];
  if(const Buffer* buffer = file.resource.force_load()) {
    return buffer->size();
  }
  return 0;
}

bool RmlUiFile::HasOutOfDateResources() {
  for(const Resource<Buffer>& resource : _resources) {
    if(_mtime < resource.slot()->mtime) {
      _mtime = resource.slot()->mtime;
      return true;
    }
  }
  return false;
}
