#include "Handle.h"

#include "../system/Memory.h"
#include "../system/intrinsics.h"

using namespace L;

static constexpr uint32_t address_bits = 48;
static constexpr uint64_t address_mask = (1ull << address_bits) - 1;
static constexpr uint64_t version_bits = 16;
static constexpr uint64_t version_mask = (1ull << version_bits) - 1;

static uint64_t* handle_objects = nullptr;
static int64_t next_index = 0;
static uint64_t freelist = address_mask;

GenericHandle::GenericHandle(void* ptr) {
  L_ASSERT((uint64_t(ptr) & address_mask) == uint64_t(ptr));

  while(freelist != address_mask) {
    const uint64_t freeslot_index = freelist;
    const uint64_t freeslot = handle_objects[freeslot_index];
    const uint64_t version = freeslot >> address_bits;
    const uint64_t new_freeslot_index = freeslot & address_mask;

    _ver_index = (version << address_bits) | freeslot_index;
    if(cas(&freelist, freeslot_index, new_freeslot_index) != freeslot_index) {
      continue;
    }
    handle_objects[freeslot_index] = (version << address_bits) | (uint64_t)ptr;
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
    const uint64_t new_freeslot_index = _ver_index & address_mask;
    const uint64_t new_freeslot = handle_objects[new_freeslot_index];

    if((new_freeslot >> address_bits) == version) {
      uint64_t old_freeslot;
      do {
        old_freeslot = freelist;
        handle_objects[new_freeslot_index] = ((version + 1) << address_bits) | old_freeslot;
      } while(cas(&freelist, old_freeslot, new_freeslot_index) != old_freeslot);
    }

    _ver_index = UINT64_MAX;
  }
}
