#include "TaskSystem.h"

#include <Windows.h>

using namespace L;

namespace L {
  void* convert_to_fiber() {
    return ConvertThreadToFiber(0);
  }
  void* create_fiber(void(*f)(void*), void* p) {
    return CreateFiber(0, f, p);
  }
  void switch_to_fiber(void* f) {
    SwitchToFiber(f);
  }
  void create_thread(void(*f)(void*), void* p) {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f, (LPVOID)p, 0, NULL);
  }
  uint32_t core_count() {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    return system_info.dwNumberOfProcessors;
  }
}
