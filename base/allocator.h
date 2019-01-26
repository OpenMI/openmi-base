#ifndef OPENMI_BASE_ALLOCATOR_H_
#define OPENMI_BASE_ALLOCATOR_H_ 

#include <limits> // numeric_limits 
#include <stdint.h>
#include <string>

namespace openmi {
/*!
 * \brief A Allocator class is an abstract interface 
 * for allocating and deallocating device memory
 */
class Allocator {
public:
  static constexpr size_t kAlignment = 32;

  virtual ~Allocator() {}

  virtual std::string Name() = 0;

  virtual void* AllocateRaw(size_t alignment, size_t num_bytes) = 0;

  virtual void DeallocateRaw(void* ptr) = 0;

  template<typename T>
  T* Allocate(size_t num_elements) {
    if (num_elements > (std::numeric_limits<size_t>::max() / sizeof(T))) {
      return nullptr;
    }

    void* ptr = AllocateRaw(kAlignment, num_elements * sizeof(T));
    T* typed_ptr = reinterpret_cast<T*>(ptr);
    return typed_ptr;
  }

  template<typename T>
  void Deallocate(T* ptr, size_t num_elements) {
    if (ptr) {
      DeallocateRaw(ptr);
    }
  }
}; // class Allocator

Allocator* cpu_allocator(std::string cpu_alloc_name = "DefaultCPUAllocator");

} // namespace 
#endif // OPENMI_BASE_ALLOCATOR_H_
