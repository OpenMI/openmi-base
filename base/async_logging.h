#ifndef OPENMI_BASE_ASYNC_LOGGING_H_
#define OPENMI_BASE_ASYNC_LOGGING_H_ 

#include <assert.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>   // bind
#include <mutex>
#include <thread>  
#include <vector>
#include "logging.h"

namespace openmi {

/*!
 * A data buffer that fixed size
 */
template <int SIZE>
class DataBuffer {
public:
  DataBuffer(): cur_(data_) { 
    SetCookie(CookieStart);
  }

  ~DataBuffer() { SetCookie(CookieEnd); }

  void Append(const char* buf, size_t len) {
    if (Avail() > len) {
      memcpy(cur_, buf, len);
      cur_ += len; 
    }
  }

  void SetCookie(void (*cookie)()) { cookie_ = cookie; }

  size_t Length() { return static_cast<size_t>(cur_ - data_); }

  const char* Data() const { return data_; }

  char* Current() { return cur_; }

  size_t Avail() const { return static_cast<size_t>(End() - cur_); }

  void Add(size_t len) { cur_ += len; }

  void Reset() { cur_ = data_; }

  void SetZero() { memset(data_, 0, sizeof(data_)); }

  std::string ToString() {
    *cur_ = '\0';
    return std::string(data_, Length());
  }
  
private:
  inline const char* End() const { 
    return data_ + sizeof(data_); 
  }

  static void CookieStart() {}
  static void CookieEnd() {}

  void (*cookie_)();
  char data_[SIZE];
  char* cur_;
}; // class DataBuffer

static const int kSmallBuffer = 4096;
static const int kLargeBuffer = 4096*1000;
static const int kMaxNumericSize = 32;

class AsyncLogging {
public:
  typedef openmi::DataBuffer<openmi::kLargeBuffer> Buffer;
  typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
  typedef BufferVector::value_type BufferPtr;

  AsyncLogging(const char* basename, const char* bin_name, off_t roll_size, LogSeverity severity = INFO, int flush_interval = 3);

  ~AsyncLogging();

  void Append(const char* msg, size_t size);

  void Start() {
    stop_ = false;
  }

  void Stop() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      stop_ = true;
    }
    thread_.join();
  }

private:
  void ConsumeLog();

private:
  const char* basename_;
  const char* bin_name_;
  off_t roll_size_;
  LogSeverity severity_;
  int flush_interval_;

  BufferPtr current_buf_;
  BufferPtr next_buf_;
  BufferVector buffers_;

  std::atomic<bool> stop_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::thread thread_; // used for consume log  
}; // class AsyncLogging

AsyncLogging::AsyncLogging(const char* basename, const char* bin_name, off_t roll_size, LogSeverity severity, int flush_interval) : 
  basename_(basename), 
  bin_name_(bin_name),
  roll_size_(roll_size), 
  severity_(severity),
  flush_interval_(flush_interval), 
  thread_(std::bind(&AsyncLogging::ConsumeLog, this)),
  current_buf_(new Buffer),
  next_buf_(new Buffer) 
{
  stop_ = false;
  current_buf_->SetZero();
  next_buf_->SetZero();
  buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
  if (! stop_) {
    Stop();
  }
}

void AsyncLogging::Append(const char* msg, size_t size) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (current_buf_->Avail() > size) {
    current_buf_->Append(msg, size);
  } else {
    buffers_.push_back(std::move(current_buf_));

    if (next_buf_) {
      current_buf_ = std::move(next_buf_);
    } else {
      current_buf_->Reset();
    }
    current_buf_->Append(msg, size);
    cond_.notify_one();
  }
}

void AsyncLogging::ConsumeLog() {
  assert(stop_ == false);
  internal::LogFile dest(basename_, bin_name_, roll_size_, g_log_file_severity_names[GetLogFileSeverityIndex(severity_)], false);
  BufferPtr buf1(new Buffer);
  BufferPtr buf2(new Buffer);
  buf1->SetZero();
  buf2->SetZero();
  BufferVector write_buffers;
  write_buffers.reserve(16);

  while (!stop_) {
    assert(buf1 && buf1->Length() == 0);
    assert(buf2 && buf2->Length() == 0);
    assert(write_buffers.empty());

    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (buffers_.empty()) {
        cond_.wait_for(lock, std::chrono::seconds(flush_interval_));  
      }
      buffers_.push_back(std::move(current_buf_));
      current_buf_ = std::move(buf1);
      write_buffers.swap(buffers_);
      if (!next_buf_) {
        next_buf_ = std::move(buf2);
      }
    }

    assert(!write_buffers.empty());

    if (write_buffers.size() > 25) {
      char buf[256];
      snprintf(buf, sizeof(buf), "Dropped log message at %s, %lu large buffes\n", "XXXXX", write_buffers.size() - 2);
      fputs(buf, stderr);
      dest.Write(buf, sizeof(buf));
      write_buffers.erase(write_buffers.begin()+2, write_buffers.end());
    }

    for (const auto& buffer: write_buffers) {
      dest.Write(buffer->Data(), buffer->Length());
    }

    if (write_buffers.size() > 2) {
      write_buffers.resize(2);
    }

    if (!buf1) {
      assert(!write_buffers.empty());
      buf1 = std::move(write_buffers.back());
      write_buffers.pop_back();
      buf1->Reset();
    }
    
    if (!buf2) {
      assert(!write_buffers.empty());
      buf2 = std::move(write_buffers.back());
      write_buffers.pop_back();
      buf2->Reset();
    }

    write_buffers.clear();
    dest.Flush();
  } // end while 
  dest.Flush();
}

class AsyncLoggingManager {
public:
  explicit AsyncLoggingManager(const char* basename, const char* bin_name, off_t roll_size)
    : basename_(basename), bin_name_(bin_name), roll_size_(roll_size) {
    async_loggings_.resize(LOG_FILE_SEVERITIES);
    for (auto async_logging: async_loggings_) {
      async_logging = nullptr;
    }
    g_async = true;
  }

  ~AsyncLoggingManager() {
    for (auto async_logging: async_loggings_) {
      if (async_logging != nullptr) {
        delete async_logging; async_logging = nullptr;
      }
    }
  }

  void Append(const char* msg, size_t size, LogSeverity severity) {
    int log_idx = GetLogFileSeverityIndex(severity);
    assert(log_idx <= LOG_FILE_SEVERITIES);
    for (int i = 0; i <= log_idx; ++i) {
      if (async_loggings_[i] == nullptr) {
        async_loggings_[i] = new AsyncLogging(
          basename_.c_str(), bin_name_.c_str(), roll_size_, 
          GetLogSeverityType(std::string(g_log_file_severity_names[i])));
        async_loggings_[i]->Start();
      }
      async_loggings_[i]->Append(msg, size);
    }
  }

private:
  std::string basename_;
  std::string bin_name_;
  off_t roll_size_;
  std::vector<AsyncLogging*> async_loggings_;
}; // class AsyncLoggingManager

} // namespace openmi
#endif // OPENMI_BASE_ASYNC_LOGGING_H_ 
