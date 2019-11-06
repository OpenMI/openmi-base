#ifndef OPENMI_BASE_RANDOM_H_
#define OPENMI_BASE_RANDOM_H_

#include <random>

namespace openmi {

static std::random_device rd;

inline float NormalRandom(float mean = 0, float stddev = 1) {
#if defined(__unix__) || defined(__linux__)
  static thread_local std::mt19937 generator(rd());
  static thread_local std::normal_distribution<float> distribution{mean, stddev};
#else 
  std::mt19937 generator(rd());
  std::normal_distribution<float> distribution{mean, stddev};
#endif 
  return distribution(generator);
}

inline float UniformRandom(float min = -1, float max = 1) {
#if defined(__unix__) || defined(__linux__)
  static thread_local std::mt19937 generator(rd());
  static thread_local std::uniform_real_distribution<float> distribution(min, max);
#else 
  std::mt19937 generator(rd());
  std::uniform_real_distribution<float> distribution(min, max);
#endif
  return distribution(generator);
}

} // namespace openmi
#endif // OPENMI_BASE_RANDOM_H_
