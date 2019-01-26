#include "sub1.h"
#include "base_factory.h"

OPENMI_REGISTER_FILE_TAG(sub1);
REGISTER_BASE(Sub1);

void Sub1::Print() {
  std::cout << "I am sub1" << std::endl;
}
