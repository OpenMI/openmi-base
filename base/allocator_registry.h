#ifndef OPENMI_BASE_ALLOCATOR_REGISTRY_H_
#define OPENMI_BASE_ALLOCATOR_REGISTRY_H_ 

#include <iostream>
#include "allocator.h"
#include "register.h"

namespace openmi { 

class AllocatorRegistry : public openmi::FunctionRegisterBase<AllocatorRegistry, std::function<openmi::Allocator*()> > {}; 

#define REGISTER_ALLOCATOR(allocator, name) \
  OPENMI_REGISTER_OBJECT_HELPER(::openmi::AllocatorRegistry, AllocatorRegistry, allocator, name) \
  .SetFunction([]() {  \
    std::cout << "device_name: " << #name << std::endl; \
    return new allocator(); })

} // namespace openmi
#endif // OPENMI_BASE_ALLOCATOR_REGISTRY_H_ 
