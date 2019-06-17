#include "type_conversion.h"

int main(int argc, char ** argv) {
  //const char* xx = "123.456";
  //const char* xx = "1.60114092E8";
  const char* xx = "160114099";
  float v = openmi::StringToNum<float>(std::string(xx));
  printf("float v: %f\n",v);
  double dv = openmi::StringToNum<double>(std::string(xx));
  printf("double v: %f\n", dv);
  long lv = (long) dv;
  printf("int v: %ld\n", lv);
  return 0;  
}
