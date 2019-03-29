#include "refcount.h"
using namespace openmi::base;

static int constructed = 0;
static int destroyed = 0;

class MyRef : public RefCounted {
public:
  MyRef(): RefCounted() { constructed++; }
  ~MyRef() override { destroyed++; }
};

int main(int argc, char** argv) {
  MyRef* ref = new MyRef();
  CHECK_EQ(constructed, 1);
  CHECK_EQ(destroyed, 0);
  ref->Ref();
  LOG(INFO) << "current ref count: " << ref->CurrentRefCount();
  CHECK_EQ(constructed, 1);
  CHECK_EQ(destroyed, 0);
  ref->Unref();
  LOG(INFO) << "current ref count: " << ref->CurrentRefCount();
  ref->Unref();
  LOG(INFO) << "current ref count: " << ref->CurrentRefCount();
  LOG(INFO) << "is one: " << ref->RefCountIsOne();
  CHECK_EQ(constructed, 1);
  CHECK_EQ(destroyed, 1);
  return 0;
}

