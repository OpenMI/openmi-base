#include "dstring.h"

int main(int argc, char** argv) {
  std::string str("auc,,logloss");
  std::vector<std::string> result;
  
  printf("split ...\n");
  openmi::Split(str, &result, ',');
  for (size_t i = 0; i < result.size(); ++i) {
    printf("i:%lu, r:%s\n", i, result[i].c_str());
  }
  
  printf("split with trim...\n");
  openmi::Split(str, &result, ',', true);
  for (size_t i = 0; i < result.size(); ++i) {
    printf("i:%lu, r:%s\n", i, result[i].c_str());
  }
  return 0;
}
