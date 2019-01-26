/*!
 *  Copyright (c) 2015 by Contributors
 * \file type_conversion.h
 * \brief basic type conversion
 * \author ZhouYong
 */
#ifndef OPENMI_BASE_TYPE_CONVERSION_H_
#define OPENMI_BASE_TYPE_CONVERSION_H_

#include <sstream>
#include <stdint.h>
#include <string>

namespace openmi {
namespace tools {
/*!
 * \brief convert string to real type. for example:
 *  std::string str = "12345678";
 *  uint32_t value = StringToNum<uint32_t>(str);
 */
template <typename T>
T StringToNum(const std::string& str) {
  std::istringstream buffer(str);
  T value; buffer >> value;
  return value;
}

/*!
 * \brief convert numeric type to string type
 */
template <typename T>
std::string NumToString(const T& value) {
  std::ostringstream buffer;
  buffer << value;
  return std::string(buffer.str());
}

} // namespace tools
} // namespace openmi
#endif // OPENMI_BASE_TYPE_CONVERSION_H_
