#ifndef OPENMI_BASE_THREADSAFE_QUEUE_H_
#define OPENMI_BASE_THREADSAFE_QUEUE_H_ 

#include <assert.h>
#include <condition_variable>
#include <mutex>  // lock_guard, unique_lock, mutex
#include <queue>

namespace openmi {
/**
 * A thread-safe, blocking queue
 */
template <typename T>
class BlockingQueue {
public:
  BlockingQueue() {}

  ~BlockingQueue() {}

  // push lock
  void Push(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    {
      queue_.push(std::move(value));
    }
    cond_.notify_all();
  }

  // wait take until queue is not empty
  void WaitTake(T* value) {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] { 
      return !queue_.empty(); 
    });
    *value = std::move(queue_.front());
    queue_.pop();
  }

  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  size_t Size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

private:
  std::queue<T> queue_;
  std::condition_variable cond_;
  mutable std::mutex mutex_;
}; // class BlockingQueue 

/**
 * A thread-safe, bounded blocking queue
 */
template <typename T>
class BoundedBlockingQueue {
public:
  explicit BoundedBlockingQueue(int capacity_size) 
    : capacity_size_(capacity_size) {}

  ~BoundedBlockingQueue() {}

  // blocking push 
  void Push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    {
      not_full_cond_.wait(lock, [this] { 
          return queue_.size() < capacity_size_; 
      });
      assert(queue_.size() < capacity_size_);
      queue_.push(std::move(value));
    }
    not_empty_cond_.notify_all();
  }

  // blocking take 
  void WaitTake(T* value) {
    std::unique_lock<std::mutex> lock(mutex_);
    {
      not_empty_cond_.wait(lock, [this] { 
        return !queue_.empty(); 
      });
      *value = std::move(queue_.front());
      queue_.pop();
    }
    not_full_cond_.notify_all();
  }

  bool Full() {
    std::lock_guard<std::mutex> lock(mutex_);
    return capacity_size_ == queue_.size();
  }

  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  size_t Size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

private:
  int capacity_size_;
  std::queue<T> queue_;
  std::condition_variable not_full_cond_;
  std::condition_variable not_empty_cond_;
  mutable std::mutex mutex_;
}; // class BoundedBlockingQueue

} // namespace openmi
#endif // OPENMI_BASE_THREADSAFE_QUEUE_H_ 
