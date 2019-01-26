#include "thread_local.h"
#include <thread>
#include <functional>

void foo(int value) {
  openmi::ThreadLocal<int> g_i;
  g_i.Value() = value;
  printf("foo tid=%llu, n=%d\n", std::this_thread::get_id(), g_i.Value());
}
void f() {
  openmi::ThreadLocalSingleton<int>::Get() = 2;
  std::cout << "f tid=" << std::this_thread::get_id() << ", get: " << openmi::ThreadLocalSingleton<int>::Get() << std::endl;
}

// g++ -std=c++11 -g -pthread thread_local_test.cc -o xx

int main() {
  openmi::ThreadLocalSingleton<int>::Get() = 1;
  std::cout << "main thread " << std::this_thread::get_id() << ", get: " << openmi::ThreadLocalSingleton<int>::Get() << std::endl;
  openmi::ThreadLocalSingleton<int>::Get() = 3;
  std::cout << "main thread " << std::this_thread::get_id() << ", get: " << openmi::ThreadLocalSingleton<int>::Get() << std::endl;

  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  std::cout << "\n ---------- \n" << std::endl;
  openmi::ThreadLocal<int> g_i;
  g_i.Value() = 10;
  std::cout << "main thread " << std::this_thread::get_id() << ", g_i: " << g_i.Value() << std::endl;
  g_i.Value() = 100;
  std::cout << "main thread " << std::this_thread::get_id() << ", g_i: " << g_i.Value() << std::endl;
  
  std::thread t3(foo, 22);
  std::thread t4(foo, 33);
  std::thread t5(std::bind(&foo, 44));
  t3.join();
  t4.join();
  t5.join();

  return 0;
}
