#include "random.h"
#include "base/logging.h"
using namespace openmi;

float mean = 0;
float variance = 1;

float min = -2;
float max = 1;

int main(int argc, char** argv) {
  float normal_mean = 0;
  float uniform_mean = 0;
  int count = 100000;
  for (int i = 0; i < count; ++i) {
    normal_mean += NormalRandom(mean, variance);
    uniform_mean += UniformRandom(min, max);
  }
  LOG(INFO) << "normal mean: " << normal_mean / count;
  LOG(INFO) << "uniform mean: " << uniform_mean / count;
  return 0;
}
