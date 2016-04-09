#ifndef DEF_L_Semaphore
#define DEF_L_Semaphore

#include <mutex>
#include <condition_variable>

namespace L {
  class Semaphore {
    private:
      std::mutex _mutex;
      std::condition_variable _condition;
      int _count;
    public:
      inline Semaphore(int count = 0) : _count(count) {}
      inline void post() {
        std::unique_lock<std::mutex> lock(_mutex);
        _count++;
        _condition.notify_one();
      }

      inline void wait() {
        std::unique_lock<std::mutex> lock(_mutex);
        while(_count == 0)
          _condition.wait(lock);
        _count--;
      }
      inline void post(int n) {while(n--)post();}
      inline void wait(int n) {while(n--)wait();}
  };
}
#endif




