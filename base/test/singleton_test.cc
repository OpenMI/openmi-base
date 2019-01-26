#include "singleton.h"
using namespace openmi;
#include <stdio.h>

class SubClass : public Singleton<SubClass> {
public:
  void Init(int i) { i_ = i; }

  int Value() {
    return i_;
  }
private:
  int i_;
};

int main(int argc, char** argv) {
  SubClass::Instance().Init(10);
  printf("i_: %d\n", SubClass::Instance().Value());
  SubClass::Instance().Init(100);
  printf("i_: %d\n", SubClass::Instance().Value());
}
