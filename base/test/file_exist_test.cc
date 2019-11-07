#include <unistd.h>
#include <stdio.h>
#include "base/timer.h"
using namespace openmi;

int access_func_perf_test(int N, const char* file) {
  int r = -1;
  for (int i = 0; i < N; ++i) {
    r = access(file, F_OK);
  }
  return r;
}

int main(int argc, char** argv) {
  const char* file = "yy.txt";
  int N = 1000000;
  Timer time;
  access_func_perf_test(N, file);
  int elapsed = time.Elapsed();
  printf("file:%s, N:%d, time:%d, avg_time:%f us\n", file, N, elapsed, elapsed*1.0/N);
  return 0;
}
