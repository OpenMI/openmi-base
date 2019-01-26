#include "allocator.h"
#include "allocator_registry.h"
#include <stdlib.h> // posix_memalign 

namespace openmi {

/*!
 * \brief a Default Allocate using malloc/free, new/delete
 */ 
class CPUAllocator: public Allocator {
public:
  CPUAllocator() {}

  ~CPUAllocator() override {}

  std::string Name() override {
    return "cpu";
  }

  // Note: alignment must be pow of 2
  void* AllocateRaw(size_t alignment, size_t num_bytes) override {
    // TODO check alignment
    void* ptr = nullptr;
    int err = posix_memalign(&ptr, alignment, num_bytes);
    if (err != 0) {
      return nullptr;
    }
    return ptr;
  }
  
  void DeallocateRaw(void* ptr) override { free(ptr);}

}; // class CPUAllocator 

REGISTER_ALLOCATOR(CPUAllocator, DefaultCPUAllocator);

Allocator* cpu_allocator(std::string cpu_alloc_name) {
  return openmi::Register<openmi::AllocatorRegistry>::Find(cpu_alloc_name)->func();
}

} // namespace openmi
