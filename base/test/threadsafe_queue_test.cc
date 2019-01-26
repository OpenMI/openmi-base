#include "threadsafe_queue.h"
#include <string>
#include <iostream>
#include <thread>
#include <unistd.h>

struct Data {
  int id; 
  std::string name;

  Data() {}

  Data(int _id, std::string _name): id(_id), name(_name) {}

  std::string DebugString() const {
    std::string rt("id:" + std::to_string(id) + ", name:" + name);
    return rt;
  }
};

size_t max_size = 10;
openmi::BoundedBlockingQueue<Data> bounded(max_size);

void PushQueue() {
  for (int i = 0; i <= max_size; ++i) {
    sleep(1);
    printf("Push %s\n", std::to_string(i*10).c_str());
    bounded.Push(Data(i, std::to_string(i*10)));
  }
}

void TakeQueue() {
  for (int i = 0; i <= max_size + 1; ++i) {
    Data d;
    bounded.WaitTake(&d);  // blocking take when i = max_size+1
    std::cout << "tid: " << std::this_thread::get_id() << ", d: " << d.DebugString() << std::endl;
  }
}

int main(int argc, char** argv) {
  openmi::BlockingQueue<Data> queue_;
  Data d1(1, "1111");
  Data d2(2, "2222");

  queue_.Push(d1);
  queue_.Push(d2);

  std::cout << "size1: " << queue_.Size() << std::endl;

  Data d3;
  Data d4;
  queue_.WaitTake(&d3);
  queue_.WaitTake(&d4);

  std::cout << "size2: " << queue_.Size() << std::endl;
  std::cout << "d3: " << d3.DebugString() << std::endl;
  std::cout << "d4: " << d4.DebugString() << std::endl;

  std::thread t1(PushQueue);
  std::thread t2(TakeQueue);

  t1.join();
  t2.join();

  return 0;
} 
