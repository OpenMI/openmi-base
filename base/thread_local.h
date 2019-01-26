#ifndef OPENMI_BASA_THREAD_LOCAL_H_ 
#define OPENMI_BASA_THREAD_LOCAL_H_ 

#include <mutex>
#include <thread>
#include <vector>

namespace openmi {

/*!
 * thread local class using pthread_key_t
 * NOTE: not singleton class
 */
template <typename T>
class ThreadLocal {
public:
  ThreadLocal() {
    if (pthread_key_create(&key_, &ThreadLocal::Destructor) != 0) {
      std::runtime_error("pthread_key_t variable create failed");
    }
  }

  ~ThreadLocal() {
    if (pthread_key_delete(key_) != 0) {
      std::runtime_error("pthread_key_t variable delete failed");
    }
  }

  T& Value() {
    T* value = static_cast<T*>(pthread_getspecific(key_));
    if (!value) {
      T* t = new T();
      if (pthread_setspecific(key_, t) != 0) {
        std::runtime_error("pthread_key_t variable set failed");
      }
      value = t;
    } else {
      std::runtime_error("pthread_key_t variable get failed.");
    }
    return *value;
  }

private:
  static void Destructor(void* v) {
    T* t = static_cast<T*>(v);
    typedef char complete_type[sizeof(T) == 0 ? -1 : 1];
    complete_type dummy_variable;
    (void) dummy_variable;
    delete t;
  }

private:
  pthread_key_t key_;
}; // class ThreadLocal

/*!
 * thread local singleton mode using __thread 
 */
template <typename T>
class ThreadLocalSingleton {
public:
  static T& Get() {
    // tptr: thread-level life cycle. NOTE: tptr must be global variable
    static __thread T* tptr = 0;
    if (tptr == nullptr) {
      printf("ThreadLocalSingleton tid: %llu, new tptr\n", std::this_thread::get_id());
      tptr = new T();
      Instance().Deleter(tptr);
    }
    return *tptr;
  }

private:
  static ThreadLocalSingleton<T>& Instance() {
    static ThreadLocalSingleton<T> inst;
    return inst;
  }
  
  ~ThreadLocalSingleton() {
    for (size_t i = 0; i < data_.size(); ++i) {
      delete data_[i];    // instance of per thread 
    }
  }

  void Deleter(T* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.push_back(ptr);
  }

private:
  std::mutex mutex_;
  // store instance of per thread
  std::vector<T*> data_;
}; // class ThreadLocalSingleton

} // namespace openmi
#endif // OPENMI_BASA_THREAD_LOCAL_H_ 
