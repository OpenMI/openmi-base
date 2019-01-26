#include "system_info.h"
using namespace openmi;

int main(int argc, char** argv) {
  printf("os_name: %s\n", SystemInfo::Instance().OsName().c_str());
  printf("os_name: %s\n", SystemInfo::Instance().Hostname().c_str());
  printf("tid: %d\n", static_cast<int>(SystemInfo::Instance().GetTID()));
  printf("tid: %d\n", static_cast<int>(SystemInfo::Instance().GetTID()));
  printf("tid: %d\n", static_cast<int>(SystemInfo::Instance().GetTID()));
  return 0;
}
