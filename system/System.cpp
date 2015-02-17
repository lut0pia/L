#include "System.h"

using namespace L;
using namespace System;

#include "Directory.h"
#include "../Exception.h"
#include "../stl.h"

#if defined L_WINDOWS
    #include <windows.h>
    #include <winsock2.h>
#elif defined L_UNIX
    #include <unistd.h>
    #include <curses.h>
#endif

String System::callGet(const String& cmd){
    String wtr = "";
    FILE* pipe = popen(cmd.c_str(),"r");
    if(!pipe) throw Exception("Couldn't open pipe in System");
    else{
        char buffer[128];
        while(!feof(pipe)) {
            if(fgets(buffer, 128, pipe) != NULL)
                wtr += buffer;
        }
        pclose(pipe);
    }
    return wtr;
}
int System::call(const String& cmd){
    return system(cmd.c_str());
}
void System::sleep(uint milliseconds){
    #if defined L_WINDOWS
        ::Sleep(milliseconds);
    #elif defined L_UNIX
        usleep(milliseconds*1000);
    #endif
}
void System::sleep(Time t){
    sleep(t.milliseconds());
}
void System::beep(uint frequency, uint milliseconds){
    #if defined L_WINDOWS
        ::Beep(frequency,milliseconds);
    #elif defined L_UNIX
        throw Exception("Cannot beep under UNIX.");
    #endif
}
void System::toClipboard(const String& data){
    #if defined L_WINDOWS
        if(OpenClipboard(NULL) && EmptyClipboard()){
            HGLOBAL tmp = GlobalAlloc(0,data.size()+1); // Allocate global memory
            GlobalLock(tmp); // Lock it
            strcpy((char*)tmp,data.c_str()); // Copy data
            GlobalUnlock(tmp); // Unlock it
            SetClipboardData(CF_TEXT,tmp);
            CloseClipboard();
        }
        else throw Exception("Couldn't open clipboard.");
    #endif
}
String System::fromClipboard(){
    #if defined L_WINDOWS
        HGLOBAL tmp;
        if(OpenClipboard(NULL) && (tmp = GetClipboardData(CF_TEXT))){
            String wtr((char*)tmp);
            CloseClipboard();
            return wtr;
        }
    #endif
    throw Exception("Cannot get clipboard data.");
}
String System::gEnv(const String& name){
    return getenv(name.c_str());
}
void System::sConsoleCursorPosition(ushort x,ushort y){
    #if defined L_WINDOWS
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD position = {(short)x,(short)y};
        SetConsoleCursorPosition(hStdout,position);
    #elif defined L_UNIX
        move(y,x);
        refresh();
    #endif
}
void System::clearConsole(){
    #if defined L_WINDOWS
        call("cls");
    #elif defined L_UNIX
        call("clear");
    #endif
}
void System::closeConsole(){
    #if defined L_WINDOWS
        FreeConsole();
    #elif defined L_UNIX
        throw Exception("Closing the console under UNIX is impossible.");
    #endif
}

String System::formatPath(String path){
    #if defined L_WINDOWS
        path = path.replaceAll("/","\\");
    #endif

    #if defined L_WINDOWS
    if(path != "\\" && (path.size()<2 || path[1]!=':'))
    #elif defined L_UNIX
    if(!path.size() || path[0]!='/')
    #endif
        path = Directory::current.gPath()+path;

    return path;
}
String System::pathDirectory(String path){
    return path.substr(0,1+path.find_last_of(slash));
}
String System::pathFile(String path){
    return path.substr(1+path.find_last_of(slash));
}

