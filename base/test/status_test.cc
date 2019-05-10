#include "errors.h"
#include "logging.h"
using namespace openmi;

int main(int argc, char** argv) {
  Status ok = Status::OK();
  LOG(INFO) << "[ok] " << ok.ToString();
  Status invalid_argument = errors::InvalidArgument("invalid args");
  ok.Update(invalid_argument);
  LOG(INFO) << "[invalid] " << invalid_argument.ToString();
  LOG(INFO) << "[updated] " << ok.ToString();
  return 0;
}
