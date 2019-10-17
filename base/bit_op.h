#ifndef OPENMI_BASE_BIT_OP_H_
#define OPENMI_BASE_BIT_OP_H_ 

#include <stdlib.h>

namespace openmi {

// nn node weight id
static uint64_t NODE_BITS = 20;

inline uint64_t GetFid(int64_t node_idx, int row_idx) {
  return (node_idx << NODE_BITS) + row_idx;
}

inline int GetNodeIdx(uint64_t fid) {
  return (fid >> NODE_BITS);
}

inline int GetRowIdx(uint64_t fid) {
  return fid & ((1L << NODE_BITS) - 1);
}

} // namespace 
#endif // OPENMI_BASE_BIT_OP_H_
