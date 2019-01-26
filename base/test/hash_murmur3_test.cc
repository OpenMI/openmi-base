#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "hash_murmur3.h"
using namespace openmi::hash;

int main(int argc, char* argv[]) {
  const char* feature_key = "openmi";
  uint32_t size = strlen(feature_key);

  uint64_t h3, h4;
  typedef MMHash128 hasher128_;
  hasher128_(feature_key, size, &h3, &h4, 0);
  printf("[128] key: %s, size: %d, h3: %llu, h4: %llu\n", feature_key, size, h3, h4);

  typedef MMHash64 hasher64_;
  uint64_t h5;
  hasher64_(feature_key, size, &h5, 0);
  printf("[64] key: %s, size: %d, h5: %llu\n", feature_key, size, h5);

  uint32_t h6;
  MMHash32(feature_key, size, &h6, 0);
  printf("[32] key: %s, size: %d, h6: %u\n", feature_key, size, h6);

  return 0;
}
