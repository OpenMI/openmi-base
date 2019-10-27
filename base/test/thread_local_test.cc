#include "../thread_local.h"
#include <thread>
#include <functional>
#include <string>
#include <unordered_map>
#include <iostream>

void foo(int value) {
  openmi::ThreadLocal<int> g_i;
  g_i.Value() = value;
  printf("before: %d,%d,%d,%d,%d,%d\n", g_i.Value(), g_i.Value(), g_i.Value(), g_i.Value(), g_i.Value(), g_i.Value());
  printf("foo tid=%llu, n=%d\n", std::this_thread::get_id(), g_i.Value());
  printf("after: %d,%d,%d,%d,%d,%d\n", g_i.Value(), g_i.Value(), g_i.Value(), g_i.Value(), g_i.Value(), g_i.Value());
}
void f() {
  openmi::ThreadLocalSingleton<int>::Get() = 2;
  std::cout << "f tid=" << std::this_thread::get_id() << ", get: " << openmi::ThreadLocalSingleton<int>::Get() << std::endl;
}

// g++ -std=c++11 -g -pthread thread_local_test.cc -o xx

int main() {
  std::cout << "\n ----- openmi::ThreadLocalSingleton<int>----- \n" << std::endl;
  openmi::ThreadLocalSingleton<int>::Get() = 1;
  std::cout << "main thread " << std::this_thread::get_id() << ", get: " << openmi::ThreadLocalSingleton<int>::Get() << std::endl;
  openmi::ThreadLocalSingleton<int>::Get() = 3;
  std::cout << "main thread " << std::this_thread::get_id() << ", get: " << openmi::ThreadLocalSingleton<int>::Get() << std::endl;

  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  std::cout << "\n ----- openmi::ThreadLocal<int>----- \n" << std::endl;
  openmi::ThreadLocal<int> g_i;
  g_i.Value() = 10;
  printf("g_i.Value 10\n");
  std::cout << "main thread " << std::this_thread::get_id() << ", g_i: " << g_i.Value() << std::endl;
  g_i.Value() = 100;
  printf("g_i.Value 100\n");
  std::cout << "main thread " << std::this_thread::get_id() << ", g_i: " << g_i.Value() << std::endl;
  
  std::thread t3(foo, 22);
  std::thread t4(foo, 33);
  std::thread t5(std::bind(&foo, 44));
  t3.join();
  t4.join();
  t5.join();

  openmi::ThreadLocal<std::unordered_map<std::string, std::string>*> map;
  // map.Value() = nullptr;
  if (map.Value() == nullptr) {
    std::cout << "map is nullptr. new it." << std::endl;
    map.Value() = new std::unordered_map<std::string, std::string>();
  }
  std::string k("1");
  std::string v("111");
  map.Value()->insert({k, v});
  map.Value()->insert({v, k});

  std::cout << "map size: " <<  map.Value()->size() << std::endl;
  map.Value()->erase(k);
  std::cout << "after erase. map size: " <<  map.Value()->size() << std::endl;
  std::cout << "map: " << map.Value()->at(v) << std::endl;

  return 0;
}
