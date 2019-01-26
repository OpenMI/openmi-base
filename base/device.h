#ifndef OPENMI_BASE_DEVICE_H_
#define OPENMI_BASE_DEVICE_H_ 

#include "allocator.h"

namespace openmi {

class Device {
public:
  Device(Allocator* allocator) {
    allocator_.reset(allocator);
  }

  virtual ~Device() {}

  Allocator* GetAllocator() {
    return allocator_.get();
  }

  virtual std::string DeviceType() = 0;

private:
  std::unique_ptr<Allocator> allocator_;
}; // class Device

} // namespace openmi
#endif // OPENMI_BASE_DEVICE_H_
