#include "bit_op.h"
using namespace openmi;
#include <iostream>

int main(int argc, char** argv) {
  int node = 1;
  int row = 0;
  std::cout << "fid:" << GetFid(node, row) << std::endl;
  return 0;
}
