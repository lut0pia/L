#ifndef DEF_L_System
#define DEF_L_System

#include "../systems.h"
#include "../macros.h"
#include "../String.h"
#include "../time.h"
#include "../geometry/Vector.h"

namespace L {
  namespace System {
    String callGet(const char*); // Makes a system call and returns the result
    int call(const char*); // Only makes a system call
    void sleep(int milliseconds);
    void sleep(const Time&);
    void beep(uint frequency, uint milliseconds);
    ullong ticks();
    void toClipboard(const String&);
    String fromClipboard();
    String env(const char*);
    String pwd();
    Vector2i screenSize();

    void consoleCursorPosition(unsigned short x,unsigned short y);
    void clearConsole();
    void closeConsole();

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

#endif
