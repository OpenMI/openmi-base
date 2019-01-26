#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <thread>

#include "date.h"
#include "thread_local.h"

static openmi::ThreadLocal<openmi::DateInfo> date_store;

void date_thread_local_test() {
  for (int i = 0; i < 3; ++i) {
    openmi::DateInfo& date_store_ = date_store.Value();
    openmi::Date::HumanDate(date_store_);
    printf("tid: %d, %d-%02d-%02d %02d:%02d:%02d.%06d\n", 
         pthread_self(),
         date_store_.year,
         date_store_.month,
         date_store_.day,
         date_store_.hour,
         date_store_.minute,
         date_store_.second,
         date_store_.usecond);
  }
}

int main(int argc, char** argv) {
  date_thread_local_test();

  std::thread t(date_thread_local_test);
  std::thread t2(date_thread_local_test);
  std::thread t3(date_thread_local_test);
  t.join();
  t2.join();
  t3.join();

  return 0;
}
