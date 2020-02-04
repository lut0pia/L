#include "Handle.h"

#include "Array.h"

using namespace L;

static Array<uint64_t> handle_objects;
static uint32_t address_bits = 48;
static uint64_t address_mask = (1ull << address_bits) - 1;
static uint64_t freelist = address_mask;

GenericHandle::GenericHandle(void* ptr) {
  if(freelist == address_mask) {
    _ver_index = handle_objects.size();
    handle_objects.push((uint64_t)ptr);
  } else {
    const uint64_t index = freelist;
    const uint64_t version = handle_objects[index] >> address_bits;
    _ver_index = (version << address_bits) | index;
    freelist = handle_objects[index] & address_mask;
    handle_objects[index] = (version << address_bits) | (uint64_t)ptr;
  }
}

void* GenericHandle::pointer() const {
  if(_ver_index != UINT64_MAX) {
    const uint64_t version = _ver_index >> address_bits;
    const uint64_t index = _ver_index & 0xffffff;
    const uint64_t object = handle_objects[index];

    // Check object version matches
    if((object >> address_bits) == version) {
      return (void*)(object & address_mask);
    }
  }

  return nullptr;
}
void GenericHandle::release() {
  if(_ver_index != UINT64_MAX) {
    const uint64_t version = _ver_index >> address_bits;
    const uint64_t index = _ver_index & 0xffffff;
    const uint64_t object = handle_objects[index];

    if((object >> address_bits) == version) {
      handle_objects[index] = ((version + 1) << address_bits) | freelist;
      freelist = index;
    }
  }
}
