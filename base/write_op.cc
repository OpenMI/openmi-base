#include "write_op.h"

namespace openmi {

Write::Write(const char* file, bool is_binary, bool is_append): written_bytes_(0) {
  is_binary_ = is_binary;
  std::ios_base::openmode mode = 
    is_append ? std::ios_base::app : std::ios_base::trunc;
  mode_ = is_binary_ ? (mode | std::ios_base::binary) : mode;
  open(file);
}

bool Write::open(const char* file) {
  ofs_.open(file, mode_);
  return ofs_.is_open() && ofs_.good();
}

bool Write::write_line(const char* msg, const int length) {
  // CHECK 
  if (msg == NULL || length == 0) {
    return true;
  }
  if (is_binary_) {
    return false;
  }
  return write(msg, length, true);
}

// write text
bool Write::write(const char* msg, const int length, bool endln) {
  if (msg == NULL || length == 0) {
    return true;
  }
  if (is_binary_) {
    return false;
  }
  if (ofs_.good()) {
    if (length > 0) {
      ofs_.write(msg, length);
      written_bytes_ += length;
    } else {
      ofs_ << msg;
      written_bytes_ += strlen(msg);
    }

    if (endln) {
      ofs_ << std::endl;
      written_bytes_ += 1;
    }
    return true;
  }
  return false;
}

bool Write::write_binary(const char* content, size_t size) {
  if (!is_binary_) {
    return false;
  }
  ofs_.write(content, size);
  written_bytes_ += size;
  return true;
}

} // namespace
