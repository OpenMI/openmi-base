#ifndef OPENMI_BASE_REFCOUNT_H_
#define OPENMI_BASE_REFCOUNT_H_ 

#include <atomic>
#include <utility>
#include "logging.h"
using namespace openmi;

namespace openmi {
namespace base {

class RefCounted {
public:
  RefCounted();

  void Ref();

  bool Unref();

  bool RefCountIsOne();

  int32_t CurrentRefCount();

protected:
  virtual ~RefCounted();

private:
  std::atomic<int32_t> ref_;

  RefCounted(const RefCounted&) = delete;
  void operator=(const RefCounted&) = delete;
}; // class RefCounted


inline RefCounted::RefCounted(): ref_(1) {}

inline RefCounted::~RefCounted() {
  //CHECK_EQ(ref_, 0);
}

inline void RefCounted::Ref() {
  CHECK_GE(ref_.load(std::memory_order_relaxed), 1);
  ref_.fetch_add(1, std::memory_order_relaxed);
}

inline bool RefCounted::Unref() {
  CHECK_GT(ref_, 0);
  // If ref_ == 1, this object is owned only by the caller. 
  ref_.fetch_sub(1, std::memory_order_relaxed);
  if (ref_.load(std::memory_order_relaxed) == 0) {
    delete this;
    return true;
  } else {
    return false;
  }
}

inline bool RefCounted::RefCountIsOne() {
  return (ref_.load() == 1);
}

inline int32_t RefCounted::CurrentRefCount() {
  return ref_.load();
}

} // namespace base 
} // namespace openmi
#endif // OPENMI_BASE_REFCOUNT_H_ 
