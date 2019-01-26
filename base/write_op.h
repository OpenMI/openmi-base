/*!
 *  Copyright 2016 by Contributors
 *  \file io_write.h
 *  \author ZhouYong
 */
#ifndef OPENMI_BASE_WRITE_OP_H_
#define OPENMI_BASE_WRITE_OP_H_

#include <fstream>
#include <iostream>
#include <sys/types.h>  // off_t
using namespace std;

namespace openmi {
/*! 
 * \brief File IO Write Operator 
 * Notice: not thread-safe
 */
class Write {
public:
  Write(const char* file, bool is_binary = false, bool is_append = false);
    
  ~Write() { close(); }
    
  bool open(const char* file);
    
  bool write_line(const char* msg, const int length = -1);
    
  bool write(const char* msg, const int length = -1, bool endln = false); // write txt
    
  bool write_binary(const char* content, size_t size);

  inline bool is_open() { return ofs_.is_open(); }
    
  inline void close() { ofs_.close(); }

  inline void flush() { ofs_.flush(); }

  inline bool is_binary() const { return is_binary_; }

  inline off_t written_bytes() { return written_bytes_; }
    
private:
  std::ofstream ofs_;
  std::ios_base::openmode mode_;
  bool is_binary_;
  off_t written_bytes_;
}; // class Write

} // namespace
#endif // OPENMI_BASE_WRITE_OP_H_
