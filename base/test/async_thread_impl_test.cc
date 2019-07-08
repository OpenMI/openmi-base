#include <assert.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h> // sleep 

class AsyncThread {
public: 
  AsyncThread() :
    thread_(std::bind(&AsyncThread::async, this)) {
    stop_ = false;
  }

  void run() {
    value1 = new std::string("I love this world!");
    cond_.notify_one();
    // sleep(1);
    printf("sleep done\n");
  }

  void stop() {
    stop_ = true;
  }

private:
  void async() {
    printf("async staring...\n");
    assert(stop_ == false);
    int count = 0;
    while (!stop_) {
      std::unique_lock<std::mutex> lock(mutex_);
      cond_.wait(lock, [this] { return value1 != nullptr; });
      printf("value1: %s\n", value1->c_str());
      delete value1; value1 = nullptr;
      if (count++ < 100)
        printf("[%d] async ...\n", count);
    }
    printf("async done...\n");
  }

private:
  std::atomic<bool> stop_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::thread thread_; // used for consume log  
  std::string* value1;
  std::string* value2;
}; // class AsyncThread

int main(int argc, char** argv) {
  AsyncThread* async = new AsyncThread();
  async->run();
  sleep(1);
  async->run();
  printf("second run done.\n");
  async->stop();
  async->run();
  sleep(1);
  printf("done\n");
  return 0;
}
