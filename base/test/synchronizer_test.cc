#include "synchronizer.h"

#include <boost/make_shared.hpp>
#include <glog/logging.h>
#include <unistd.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>

using namespace apache::thrift::concurrency;
using namespace openmi;

// Thread level task 
class Task: public ::apache::thrift::concurrency::Runnable {
public:
  Task(Synchronizer& sync, int i): sync_(sync), thread_id_(i) {}
  ~Task() {}
  void run() override;  // concrete task logic

private:
  Synchronizer& sync_;
  int thread_id_;
};

void Task::run() {
  LOG(INFO) << "thread " << thread_id_;
  //bool task_result = thread_id_ == 1 ? false : true;
  bool task_result = true;
  sync_.AddFinishOne(task_result);
}

int num_thread = 5;

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_stderrthreshold = google::INFO;
  FLAGS_colorlogtostderr = true;
  boost::shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager(num_thread);
  boost::shared_ptr<PosixThreadFactory> thread_factory(new PosixThreadFactory());
  thread_manager->threadFactory(thread_factory);
  thread_manager->start(); 

  Synchronizer sync_;
  sync_.Clear();
  for (int i = 0; i < num_thread; ++i) {
    auto task = boost::make_shared<Task>(sync_, i);
    thread_manager->add(task);
  }
  sync_.Wait(num_thread);

  LOG(INFO) << "done";
  return 0;
}
