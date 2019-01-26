#include "sub2.h"
#include "base_factory.h"

OPENMI_REGISTER_FILE_TAG(sub2);
REGISTER_BASE(Sub2);

void Sub2::Print() {
  std::cout << "I am sub2" << std::endl;
}
