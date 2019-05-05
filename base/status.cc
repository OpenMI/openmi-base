#include "status.h"
#include <assert.h>
#include <stdio.h>

using namespace openmi;

namespace openmi {

Status::Status(Code code, std::string msg) {
  assert(code != openmi::OK);
  state_ = std::unique_ptr<State>(new State);
  state_->code = code;
  state_->msg = msg;
}

void Status::Update(const Status& new_status) {
  if (ok()) {
    *this = new_status;
  }
}

void Status::SlowCopyFrom(const State* s) {
  if (s == nullptr) {
    state_ = nullptr;
  } else {
    state_ = std::unique_ptr<State>(new State(*s));
  }
}

std::string error_name(Code code) {
  switch (code) {
    case openmi::OK:
      return "OK"; 
      break;
    case openmi::INVALID_ARGUMENT:
      return "Invalid argument"; 
      break;
    case openmi::NOT_FOUND:
      return "Not found"; 
      break;
    case openmi::OUT_OF_RANGE:
      return "Out of range"; 
      break;
    case openmi::UNKNOWN:
      return "Unknown"; 
      break;
      // TODO 
    default:
      char tmp[30];
      snprintf(tmp, sizeof(tmp), "Unknown code(%d)", static_cast<int>(code));
      return tmp;
      break;
  }
}

std::string Status::ToString() const {
  if (state_ == nullptr) {
    return "OK";
  } else {
    std::string result(error_name(code()));
    result += ": ";
    result += state_->msg;
    return result;
  }
}

std::ostream& operator<<(std::ostream& os, const Status& s) {
  os << s.ToString();
  return os;
}

}
