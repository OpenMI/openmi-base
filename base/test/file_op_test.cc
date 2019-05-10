#include "file_op.h"
using namespace openmi;
#include <iostream>
#include <cstdlib>  // system


int main(int argc, char** argv) {
  std::string path("openmi");
  std::string cmd = "rm -rf " + path + " || true";
  printf("cmd:%s\n", cmd.c_str());
  int rt = system(cmd.c_str());
  if (rt == 0) {
    printf("system exec success\n"); 
  }
  
  if (FileOp::access_dir(path.c_str())) {
    printf("%s dir exists. rm it\n", path.c_str());
    if (FileOp::rm_dir(path.c_str())) {
      printf("%s rm success\n", path.c_str());
    } else {
      printf("%s rm failed\n", path.c_str());
    }
  } else {
    printf("%s dir not exists\n", path.c_str());
    if (FileOp::mk_dir(path.c_str())) {
      printf("%s create success\n", path.c_str());
    } else {
      printf("%s create failed\n", path.c_str());
    }
  }
  return 0;
}
