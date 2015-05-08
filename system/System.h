#ifndef DEF_L_System
#define DEF_L_System

#include "../systems.h"
#include "../macros.h"
#include "../stl/String.h"
#include "../time.h"
#include "../geometry/Point.h"

namespace L{
    namespace System{
        String callGet(const String&);      // Makes a system call and returns the result
        int call(const String&);                 // Only makes a system call
        void sleep(int milliseconds);
        void sleep(const Time&);
        void beep(uint frequency, uint milliseconds);
        ullong ticks();
        void toClipboard(const String&);
        String fromClipboard();
        String gEnv(const String&);
        Point2i screenSize();

        void sConsoleCursorPosition(unsigned short x,unsigned short y);
        void clearConsole();
        void closeConsole();

        String formatPath(String);
        String pathDirectory(String);
        String pathFile(String);

        #if defined L_WINDOWS
            const char slash = '\\';
        #elif defined L_UNIX
            const char slash = '/';
        #endif

        inline void openURL(const String& url){
            #if defined L_WINDOWS
                call("start "+url);
            #elif defined L_UNIX
                call("xdg-open "+url);
            #endif
        }
    }
}

#endif
