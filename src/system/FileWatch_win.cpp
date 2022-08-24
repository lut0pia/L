#include <windows.h>

#include "FileWatch.h"
#include "Memory.h"

using namespace L;

struct FileWatchWin {
  HANDLE file;
  OVERLAPPED overlapped;
  uint8_t change_buf[1024];
  FileWatchWin(const char* root_path) {
    file = CreateFile(root_path,
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
      NULL);
    L_ASSERT(file != INVALID_HANDLE_VALUE);
    overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

    queue_event();
  }
  ~FileWatchWin() {
    CloseHandle(file);
  }
  void update(Array<String>& changes) {
    if(WaitForSingleObject(overlapped.hEvent, 0) == 0) {
      DWORD bytes_transferred;
      GetOverlappedResult(file, &overlapped, &bytes_transferred, FALSE);

      FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;
      for(;;) {
        char utf8_name[sizeof(change_buf)]{};

        // Convert to utf-8
        WideCharToMultiByte(CP_UTF8, 0, event->FileName, event->FileNameLength / sizeof(wchar_t), utf8_name, sizeof(utf8_name), NULL, NULL);

        // Replace backslashes with forward slashes
        for(char* c = utf8_name; *c != '\0'; c++) {
          if(*c == '\\') {
            *c = '/';
          }
        }

        changes.push(utf8_name);

        // Are there more events to handle?
        if(event->NextEntryOffset) {
          *((uint8_t**)&event) += event->NextEntryOffset;
        } else {
          break;
        }
      }

      queue_event();
    }
  }
  void queue_event() {
    ReadDirectoryChangesW(
      file, change_buf, sizeof(change_buf), TRUE,
      FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_LAST_WRITE,
      NULL, &overlapped, NULL);
  }
};

FileWatch::FileWatch(const char* root_path)
  : _root(root_path) {
  _pimpl = Memory::new_type<FileWatchWin>(root_path);
}
FileWatch::~FileWatch() {
  Memory::delete_type((FileWatchWin*)_pimpl);
}
void FileWatch::update() {
  ((FileWatchWin*)_pimpl)->update(_changes);
}
