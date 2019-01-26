#include "async_logging.h"
#include "timer.h"
#include <unistd.h>

using namespace openmi;

off_t kRollSize = 1024*1024*256;

openmi::AsyncLoggingManager* g_asynclog = nullptr;

void asyncOutput(const char* msg, size_t len, LogSeverity severity) {
  g_asynclog->Append(msg, len, severity);
}

const char* msg = "HELLO, 123456789098765432101234567890!!!! ";

void Bench() {
  openmi::LogMessage::SetSendMethod(asyncOutput);

  int epoch = 100;
  int M = 1000000;

  openmi::Timer timer;
  for (int i = 0; i < epoch; ++i) {
    for (int j = 0; j < M; ++j) {
      LOG(INFO) << msg << i*M+j;
    }
    LOG(ERROR) << msg << ",epoch:" << epoch;
  }
  LOG(ERROR) << msg;
  printf("count: %d, consume time: %d\n", epoch*M, timer.Elapsed());
}

int main(int argc, char** argv) {
  const char* basename = "test_async_logging";
  const char* bin_name = "async";
  openmi::AsyncLoggingManager async_log(basename, bin_name, kRollSize);
  g_asynclog = &async_log;

  Bench();

  return 0;
}
