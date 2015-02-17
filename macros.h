#ifndef DEF_L_macros
#define DEF_L_macros

    #if defined WIN32
        #define L_WINDOWS
    #elif defined __unix__
        #define L_UNIX
    #else
        #error Unknown OS
    #endif

    #define typeof __typeof__ // static operator replaced by type name of expression
    #if defined L_WINDOWS
        #define popen _popen
        #define pclose _pclose
    #endif

    #define L_Auto(n,v) typeof(v) n(v)
    #define L_Iter(name,it) for(L_Auto(it,(name).begin());it!=(name).end();++it)
    #define L_RevIter(name,it) for(L_Auto(it,(name).rbegin());it!=(name).rend();++it)
    /*
    #define L_NoCopy(class) class(const class&) = delete; \
                            class& operator=(const class&) = delete;
    */
    #define L_NoCopy(class) class(const class&); \
                            class& operator=(const class&);

    namespace L{
        typedef unsigned char byte;
        typedef unsigned short ushort;
        typedef unsigned int uint;
        typedef unsigned long ulong;
        typedef unsigned long long ullong;
    }

#endif

