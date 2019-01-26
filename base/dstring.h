/*!
 *  Copyright (c) 2015 by Contributors
 *  \file dstring.h
 *  \brief string basic op 
 *  \author ZhouYong
 */
#ifndef OPENMI_BASE_DSTRING_H_
#define OPENMI_BASE_DSTRING_H_

#include <string>
#include <sstream>
#include <vector>

namespace openmi {

inline void Split(const std::string& str, std::vector<std::string>* ret, const char delimeter=' ', bool trim=false) {
  ret->clear();
  std::istringstream is(str);
  std::string item;
  while(std::getline(is, item, delimeter)) {
    if (trim && item.length() == 0) continue;
    ret->push_back(item);
  }
} // Split 

} // namespace openmi
#endif // OPENMI_BASE_DSTRING_H_
