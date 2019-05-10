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

#define NAME(code) #code

std::string error_name(Code code) {
  switch (code) {
    case openmi::OK:
      return NAME(OK); 
      break;
    case openmi::INVALID_ARGUMENT:
      return NAME(INVALID_ARGUMENT);
      break;
    case openmi::NOT_FOUND:
      return NAME(NOT_FOUND);
      break;
    case openmi::OUT_OF_RANGE:
      return NAME(OUT_OF_RANGE);
      break;
    case openmi::UNKNOWN:
      return NAME(UNKNOWN);
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
