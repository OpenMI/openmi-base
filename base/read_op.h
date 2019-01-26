/*!
 *  Copyright 2016 by Contributors
 *  \file read_op.h
 *  \brief read op 
 *  \author ZhouYong
 */
#ifndef OPENMI_BASE_READ_OP_H_
#define OPENMI_BASE_READ_OP_H_

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdexcept> // runtime_error

namespace openmi {

/*! \brief File IO Read Operator */
class Read {
public:
  explicit Read(const char* file, bool is_binary=false);
    
  ~Read() { close(); }

  bool open(const char* file, bool is_binary=false);
  
  bool get_line(std::string& line);
  
  inline bool is_open() { return ifs_.is_open(); }
  
  inline void close() { ifs_.close(); } 

private:
  std::ifstream ifs_;
  bool is_binary_;
}; // class Read

/*! 
 * \brief mmap based on memory read operator. 
 *        read data saved as void*
 */
class MMap {
public:
  MMap(): store_(0), size_(0) {}
  
  ~MMap() { close(); }
    
  bool load(const char* path);
    
  void close();
  
  inline void* data() const { return store_; }

  inline uint64_t size() const { return size_; } 

  std::string TextContent() {
    return std::string((char*)store_);
  }

  std::string TextContent(const std::string path) {
    if (!load(path.c_str())) {
      std::runtime_error("load file failed. file: " + path);
    }
    return std::string((char*)store_);
  }
  
private:
  void* store_;
  uint64_t size_;
}; // class MMap

} // namespace
#endif // OPENMI_BASE_READ_OP_H_
