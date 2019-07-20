#include <stdlib.h>               // posix_memalign 
#include "allocator.h"
#include "allocator_registry.h"
using namespace openmi;

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
    return static_cast<void*>(new char[num_bytes]);
    /*
    // TODO check alignment
    void* ptr = nullptr;
    int err = posix_memalign(&ptr, alignment, num_bytes);
    if (err != 0) {
      return nullptr;
    }
    return ptr;
    */
  }
  
  void DeallocateRaw(void* ptr) override { 
    delete[] static_cast<char*>(ptr);
    //free(ptr);
  }

}; // class CPUAllocator 

REGISTER_ALLOCATOR(CPUAllocator, DefaultCPUAllocator);

Allocator* cpu_allocator(std::string cpu_alloc_name) {
  const AllocatorRegistry* allocator_registry = Register<AllocatorRegistry>::Find(cpu_alloc_name);
  if (allocator_registry == nullptr) {
    LOG(ERROR) << "allocator not exists. alloc name:" << cpu_alloc_name;
    return nullptr;
  }
  return allocator_registry->func();
}

//OPENMI_REGISTER_FILE_TAG(cpu_allocator);

} // namespace openmi
