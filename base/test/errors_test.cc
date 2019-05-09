#include <stdio.h>
#include "errors.h"
#include "logging.h"
using namespace openmi;

int main(int argc, char** argv) {
  Status s = errors::InvalidArgument("Invalid x=10");
  printf("s.ToString: %s, is invalid argument: %d\n", s.ToString().c_str(), errors::IsInvalidArgument(s));

  Status not_found = errors::NotFound("Not Found index 0");
  printf("not_found.msg: %s, is: %d\n", not_found.ToString().c_str(), errors::IsNotFound(not_found));

  LOG(DEBUG) << "DEBUG: " << not_found.ToString();
  LOG(INFO) << "INFO: " << not_found.ToString();
  LOG(WARN) << "WARN: " << not_found.ToString();
  LOG(ERROR) << "ERROR: " << not_found.ToString();

  return 0;
}
