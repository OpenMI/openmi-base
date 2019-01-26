#include <stdio.h>
#include <vector>
#include <future>
#include "timer.h"
#include "thread_pool.h"
#include "allocator.h"
using namespace openmi;

static int epoch = 100;
static int N = 1000000;
static int num_thread = 5;

void default_malloc() {
  for (int i = 0; i < epoch * N; ++i) {
    double* ss = new double[10];
    delete[] ss;
    if (i % N == 0) {
      printf("i: %d\n", i);
    }
  }
}

Allocator* cpu_alloc = openmi::cpu_allocator();

void custom_malloc() {
  for (int i = 0; i < epoch * N; i++) {
    double* ss = cpu_alloc->Allocate<double>(10);
    cpu_alloc->Deallocate<double>(ss, 10);
    if (i % N == 0) {
      printf("i: %d\n", i);
    }
  }
}

int main(int argc, char** argv) {
  openmi::Timer timer;
  //default_malloc();
  custom_malloc();
  /*
  openmi::ThreadPool::Instance().Init(num_thread);
  std::vector<std::future<void> > vec;
  for (int i = 0; i < num_thread; ++i) {
    vec.push_back(openmi::ThreadPool::Instance().AddTask([i] {
      default_malloc(); 
    }));
  }
  */
  printf("default_malloc: %d\n", timer.Elapsed());
  return 0;
}
