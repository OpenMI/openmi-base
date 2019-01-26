#include "type_conversion.h"

int main(int argc, char ** argv) {
  const char* xx = "123.456";
  float v = openmi::tools::StringToNum<float>(std::string(xx));
  printf("v: %f\n",v);
  return 0;  
}
