#ifndef OPENMI_BASE_STATUS_H_
#define OPENMI_BASE_STATUS_H_ 

#include <functional>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <string> 

namespace openmi {

enum Code {
  OK = 0,
  INVALID_ARGUMENT = 1,
  NOT_FOUND = 2,
  OUT_OF_RANGE = 3,
  UNKNOWN = 4,
  // TODO 
};

class Status {
public:
  Status(): state_(nullptr) {}

  Status(Code code, std::string msg);

  // Copy the specified status. 
  Status(const Status& s);
  void operator=(const Status& s);

  static Status OK() { return Status(); }

  // Return true if the status indicates success. 
  bool ok() const { return state_ == nullptr; }
  
  Code code() const { return ok() ? openmi::OK : state_->code; }

  const std::string error_message() const {
    return ok() ? "not error message" : state_->msg;
  }

  bool operator==(const Status& x) const;
  bool operator!=(const Status& x) const;

  void Update(const Status& new_status);

  std::string ToString() const;

private:
  struct State {
    Code code;
    std::string msg;
  };

  void SlowCopyFrom(const State* src);

  std::unique_ptr<State> state_;
}; // class Status

inline Status::Status(const Status& s) 
  : state_((s.state_ == nullptr) ? nullptr : new State(*s.state_)) {}

inline void Status::operator=(const Status& s) {
  if (state_ != s.state_) {
    SlowCopyFrom(s.state_.get());
  }
}

inline bool Status::operator==(const Status& s) const {
  return (this->state_ == s.state_) || (ToString() == s.ToString());
}

inline bool Status::operator!=(const Status& s) const { return !(*this == s); }

std::ostream& operator<<(std::ostream& os, const Status& s);

typedef std::function<void(const Status&)> StatusCallback;

} // namespace openmi
#endif 
