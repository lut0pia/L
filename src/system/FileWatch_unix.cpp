#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "../container/Table.h"
#include "FileWatch.h"
#include "Memory.h"

using namespace L;

struct FileWatchUnix {
  int fd;
  Table<int, String> directories;
  FileWatchUnix(const char* root_path) {
    fd = inotify_init();
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    watch_dir(root_path);
  }
  ~FileWatchUnix() {
    for(const auto& dir : directories) {
      inotify_rm_watch(fd, dir.key());
    }
    close(fd);
  }
  void watch_dir(const String& path) {
    int wd = inotify_add_watch(fd, path, IN_MODIFY);
    directories[wd] = path;

    DIR* dir = opendir(path);
    while(struct dirent* dp = readdir(dir)) {
      if(dp->d_type == DT_DIR
        && strcmp(dp->d_name, ".") != 0
        && strcmp(dp->d_name, "..") != 0) {
        if(path == ".") {
          watch_dir(dp->d_name);
        } else {
          watch_dir(path + dp->d_name);
        }
      }
    }
    closedir(dir);
  }
  void update(Array<String>& changes) {
    uint8_t buffer[1024];
    const ssize_t len = read(fd, buffer, sizeof(buffer));
    if(len == EAGAIN) {
      return;
    }

    for(long i = 0; i < len;) {
      struct inotify_event* event = (struct inotify_event*)(buffer + i);
      if(event->len) {
        const String directory = directories[event->wd];
        if(directory == ".") {
          changes.push(event->name);
        } else {
          changes.push(directory + "/" + event->name);
        }
      }
      i += sizeof(struct inotify_event) + event->len;
    }
  }
};

FileWatch::FileWatch(const char* root_path)
  : _root(root_path) {
  _pimpl = Memory::new_type<FileWatchUnix>(root_path);
}
FileWatch::~FileWatch() {
  Memory::delete_type((FileWatchUnix*)_pimpl);
}
void FileWatch::update() {
  ((FileWatchUnix*)_pimpl)->update(_changes);
}
