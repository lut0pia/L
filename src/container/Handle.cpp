#include "Handle.h"

#include "Array.h"

using namespace L;

static uint64_t* handle_objects = nullptr;
static int64_t next_index = 0;
static uint32_t address_bits = 48;
static uint64_t address_mask = (1ull << address_bits) - 1;
static uint64_t freelist = address_mask;

GenericHandle::GenericHandle(void* ptr) {
  while(freelist != address_mask) {
    const uint64_t index = freelist;
    const uint64_t version = handle_objects[index] >> address_bits;
    _ver_index = (version << address_bits) | index;
    if(cas(&freelist, index, handle_objects[index] & address_mask) != index) {
      continue;
    }
    handle_objects[index] = (version << address_bits) | (uint64_t)ptr;
    return;
  }

  // Allocate memory for handle slots
  if(handle_objects == nullptr) {
    handle_objects = (uint64_t*)Memory::virtual_alloc(1ull << 20);
  }

  // Allocate new handle slot
  _ver_index = atomic_add(&next_index, int64_t(1));
  handle_objects[_ver_index] = uint64_t(ptr);
}

void* GenericHandle::pointer() const {
  if(_ver_index != UINT64_MAX) {
    const uint64_t version = _ver_index >> address_bits;
    const uint64_t index = _ver_index & address_mask;
    const uint64_t object = handle_objects[index];

    // Check object version matches
    if((object >> address_bits) == version) {
      return (void*)(object & address_mask);
    }
  }

  return nullptr;
}
uint64_t GenericHandle::index() const {
  return _ver_index & address_mask;
}
void GenericHandle::release() {
  if(_ver_index != UINT64_MAX) {
    const uint64_t version = _ver_index >> address_bits;
    const uint64_t new_free = _ver_index & address_mask;
    const uint64_t object = handle_objects[new_free];

    if((object >> address_bits) == version) {
      uint64_t old_free;
      do {
        old_free = freelist;
        handle_objects[new_free] = ((version + 1) << address_bits) | old_free;
      } while(cas(&freelist, old_free, new_free) != old_free);
    }

    _ver_index = UINT64_MAX;
  }
}
