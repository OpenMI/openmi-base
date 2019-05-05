#ifndef OPENMI_BASE_ERRORS_H_
#define OPENMI_BASE_ERRORS_H_

#include <sstream>
#include "status.h"

namespace openmi {
namespace errors {

#define DECLARE_ERROR(FUNC, CODE)                         \
  ::openmi::Status FUNC(std::string msg) {                \
    return ::openmi::Status(::openmi::CODE, msg);         \
  }                                                       \
  inline bool Is##FUNC(const ::openmi::Status& status) {  \
    return status.code() == ::openmi::CODE;              \
  }

DECLARE_ERROR(InvalidArgument, INVALID_ARGUMENT)
DECLARE_ERROR(NotFound, NOT_FOUND)
DECLARE_ERROR(OutOfRange, OUT_OF_RANGE)

#undef DECLARE_ERROR

} // namespace errors
} // namespace openmi
#endif // OPENMI_BASE_ERRORS_H_ 
