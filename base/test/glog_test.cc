#include <glog/logging.h>
#include <assert.h>

int main(int argc, char** argv) {
  google::InitGoogleLogging("ss");
  FLAGS_logtostderr = true;
  FLAGS_alsologtostderr = true;
  FLAGS_stderrthreshold = google::INFO;
  const char* c = "I love this world!";
  DLOG(INFO) << c;
  LOG(INFO) << c;
  assert(1 != 1);
  LOG(WARNING) << c;
  LOG(ERROR) << c;
  return 0;
}
