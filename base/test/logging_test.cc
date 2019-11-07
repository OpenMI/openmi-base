#include "logging.h"
#include "timer.h"
#include <thread>

using namespace openmi;

//static int N = 10;
static int NN = 10000;
//static int NN = 1000;

const char* msg = "openmi logging";

void default_logging() {
  printf("default_logging. file:%s, line:%d\n", __FILE__, __LINE__);
  LOG(INFO) << msg;
  LOG(WARN) << msg;
  LOG(ERROR) << msg;
  DLOG(INFO) << "DLOG " << msg;
  LOG(FATAL) << "fatal " << msg; 
}

void init_logging(char** argv) {
  InitLogging(argv[0]);
  g_log_dir = "log";
  g_alsologtostderr = true;
  LOG(INFO) << msg << " init";
  LOG(WARN) << msg << " init";
  LOG(ERROR) << msg << " init";
  ShutdownLogging();
}

void logging_bench() {
  for (int i = 0; i < NN; ++i) {
    LOG(INFO) << msg;
    //LOG(WARN) << msg;
    //LOG(ERROR) << msg;
    //sleep(1);
  }
}

void multi_thread_logging(char** argv) {
  openmi::Timer timer;
  
  InitLogging(argv[0]);
  g_log_dir = "log";
  g_alsologtostderr = false;

  std::thread t1(logging_bench);
  std::thread t2(logging_bench);
  std::thread t3(logging_bench);
  std::thread t4(logging_bench);
  std::thread t5(logging_bench);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();

  ShutdownLogging();
  int end_time = timer.Elapsed();
  int thread_num = 5;
  printf("[%d] time:[%d] speed:%f\n", thread_num * NN, end_time, thread_num * NN * 1.0 / (end_time*1.0 / 1e6));
}

// custom send method 
internal::LogFile* g_logfile = nullptr;

void send_method(const char* msg, size_t size, LogSeverity severity) {
  g_logfile->Write(msg, size);
}

void custom_send_method(char** argv) {
  InitLogging(argv[0]);
  const char* base_dirname = "log";
  const char* bin_name = "logging_test";
  internal::LogFile log_file(base_dirname, bin_name, 1024*1024*1024);
  g_logfile = &log_file;
  openmi::LogMessage::SetSendMethod(send_method);
  logging_bench();

  ShutdownLogging();
}

void check_binary_op() {
  CHECK_EQ(1, 2) << "1 != 2";
} 

int main(int argc, char** argv) {
  //default_logging();
  //init_logging(argv);
  multi_thread_logging(argv);
  //custom_send_method(argv); 
  //check_binary_op();
  return 0;
}
