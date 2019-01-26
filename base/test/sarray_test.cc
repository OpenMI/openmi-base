#include "sarray.h"

using namespace openmi;

int main(int argc, char** argv) {
  SArray<float> s(100, 10);
  printf("s.data: %s\ns.size: %lu\n", DebugStr<float>(s.data(), 10).c_str() , s.size());

  SArray<float> sv(s.data(), s.size());
  printf("sv.data: %s\nsv.size: %lu\n", DebugStr<float>(sv.data(), 10).c_str() , sv.size());
}
