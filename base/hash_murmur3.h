/*!
 *  Copyright 2017 by Contributors
 *  \file hash_murmur3.h
 *  \brief A hash algorithms - murmurhash3
 *  \author ZhouYong
 */
#ifndef OPENMI_BASE_HASH_MURMUR3_H_
#define OPENMI_BASE_HASH_MURMUR3_H_

#include <stdint.h>

namespace openmi {
namespace hash {

/*! \brief 128 bit mm3 hasher */
class MMHash128 {
public:
  /*! \brief default constructor */
  MMHash128() {}
  /*! \brief constructor by parameter */
  MMHash128(const void* key, const int len, void* hash1, void* hash2, const uint32_t seed = 0) {
    hash128(key, len, hash1, hash2, seed);
  }

  /*! \brief complentation of mm3-hash128 bit. generate 2 64-bit hash value */
  void hash128(const void* key, const int len, void* hash1, void* hash2, const uint32_t seed = 0);
  
  /*! \brief complentation of mm3-hash128 bit. geneate 1 128-bit hash value */
  void hash128(const void* key, const int len, void* hash, const uint32_t seed = 0);
}; // class MMHash128

/*! \brief 64 bit mm3 hasher */
class MMHash64 {
public:
  /*! \brief constructor by parameter */
  MMHash64(const void* data, const int len, uint64_t* hash, const uint32_t seed = 0) {
    uint64_t hash2;
    MMHash128(data, len, hash, &hash2, seed);
  }
}; // class MMHash64

/*! \brief 32 bit mm3 hasher */
class MMHash32 {
public:
  /*! \brief constructor by parameter */
  MMHash32(const void* key, const int len, void* hash, const uint32_t seed = 0) {
    hash32(key, len, hash, seed);
  }

  /*! 
   * \brief complementation of mm3-hash32 bit. generate 1 32-bit hash value
   */
  void hash32(const void* key, const int len, void* hash, const uint32_t seed = 0);
  
  /*! \brief 32-bit hash */
  uint32_t hash32(const void* key, const int len, const uint32_t seed = 0);

}; // class MMHash32

} // namespace hash
} // namespace openmi
#endif // OPENMI_BASE_HASH_MURMUR3_H_
