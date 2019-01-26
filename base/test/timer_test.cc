#include "timer.h"
#include "logging.h"
using namespace openmi;

#include <unistd.h>

void TimerTest() {
  Timer timer;
  sleep(1);
  int t1 = timer.Elapsed(false);
  LOG(INFO) << "[1] sleep(1) t1: " << t1;
  
  sleep(2);
  int t2 = timer.Elapsed();
  LOG(INFO) << "[2] sleep(2) t1 + t2: " << t2;
  DLOG(INFO) << "DLOG mode .....";
}

void TimerManagerTest() {
  TimerManagerPtr timer_manager_ptr = std::make_shared<TimerManager>();
  timer_manager_ptr->AddTimer("timer1");
  timer_manager_ptr->AddTimer("timer2");
  
  sleep(1);
  LOG(INFO) << "timer1: " << timer_manager_ptr->GetTimer("timer1")->Elapsed();
  LOG(INFO) << "timer2: " << timer_manager_ptr->GetTimer("timer2")->Elapsed(false);
  sleep(1);
  LOG(INFO) << "timer2: " << timer_manager_ptr->GetTimer("timer2")->Elapsed();
}

int main(int argc, char** argv) {
  TimerTest();
  TimerManagerTest();
  return 0;
}
