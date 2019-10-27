#ifndef OPENMI_BASE_THREAD_POOL_H_
#define OPENMI_BASE_THREAD_POOL_H_ 

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "threadsafe_queue.h"

namespace openmi {
/*!
 * \brief a general thread pool, include return result type
 */
class ThreadPool {
public:
  ThreadPool(): num_thread_(0) {}

  ~ThreadPool(); 

  /*!
   * singleton mode
   */
  static ThreadPool& Instance() {
    static ThreadPool thread_pool;
    return thread_pool;
  }

  void Init(size_t num_thread);

  int NumThread() { return num_thread_; }

  /*! 
   * \brief add task event
   * \param class F function 
   * \param Args arguments
   */
  template<class F, class... Args>
  auto AddTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

  void Join();

  void Stop();

private:
  int num_thread_;
  std::vector<std::thread> worker_threads_;
  openmi::BlockingQueue<std::function<void()> > ready_tasks_; 

  std::mutex mutex_;
  std::condition_variable cond_;
  std::atomic<bool> stop_;
    
}; // class ThreadPool

inline void ThreadPool::Init(size_t num_thread) {
  num_thread_ = num_thread;
  stop_ = false;
  for (size_t i = 0; i < num_thread; ++i) {
    worker_threads_.emplace_back([this] {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lk(this->mutex_);
          this->cond_.wait(lk, [this] { 
            return this->stop_ || !this->ready_tasks_.Empty(); 
          });
          if (this->stop_ && this->ready_tasks_.Empty()) return;
          this->ready_tasks_.WaitTake(&task);
        }
        task();
      }
    });
  }
} // ThreadPool::ThreadPool 

inline void ThreadPool::Join() {
  for (auto&& thread: worker_threads_) {
    thread.join();
  }
}

inline void ThreadPool::Stop() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    stop_ = true;
  }
  cond_.notify_all();
}

inline ThreadPool::~ThreadPool() {
  printf("ThreadPool::~ThreadPool() begin\n");
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (stop_) return;
    stop_ = true;
  }
  cond_.notify_all();
  Join();
  printf("ThreadPool::~ThreadPool() done\n");
}

template<class F, class... Args>
auto ThreadPool::AddTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;
  
  auto task = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  
  std::future<return_type> rt = task->get_future();
  {
    std::unique_lock<std::mutex> lk(mutex_);
    if (stop_) {
      throw std::runtime_error("ThreadPool has stopped. it's not allow to AddTask.");
    }
    ready_tasks_.Push([task]() { (*task)(); });
  }
  cond_.notify_one();
  return rt;
} // ThreadPool::AddTask

} // namespace openmi
#endif // OPENMI_BASE_THREAD_POOL_H_
