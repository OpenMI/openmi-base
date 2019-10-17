/*!
 *  Copyright (c) 2016 by Contributors
 *  \file shell_utils.h
 *  \brief c++ callback shell command
 */
#ifndef OPENMI_BASE_SHELL_UTIL_H_
#define OPENMI_BASE_SHELL_UTIL_H_ 

#include <string>
#include <vector>
#include <string.h>

namespace openmi {

int Exec(const std::string cmd, std::vector<std::string>& out) {
  FILE *fp = popen(cmd.c_str(), "r");
  if (!fp) {
    return -1;
  }
  char line[1024*8];
  while (fgets(line, sizeof(line), fp) != NULL) {
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }
    out.push_back(line);
  }
  pclose(fp);
  return 0;
} // Exec

} // namespace 
#endif // OPENMI_BASE_SHELL_UTIL_H_
