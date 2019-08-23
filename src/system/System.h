#pragma once

#include "../macros.h"
#include "../text/String.h"
#include "../time/Time.h"
#include "../math/Vector.h"

namespace L {
  namespace System {
    int call(const char*, String& output); // Makes a system call and fills output
    int call(const char*); // Only makes a system call
    void sleep(int milliseconds);
    void sleep(const Time&);
    String pwd();
    Vector2i screenSize();

    String formatPath(String);
    String pathDirectory(const String&);
    String pathFile(const String&);

    void* alloc(size_t);
    void dealloc(void*);

#if defined L_WINDOWS
    const char slash = '\\';
#elif defined L_UNIX
    const char slash = '/';
#endif

    inline void openURL(const String& url) {
#if defined L_WINDOWS
      call("start "+url);
#elif defined L_UNIX
      call("xdg-open "+url);
#endif
    }
  }
}
