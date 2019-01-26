/*!
 *  Copyright (c) 2016 by Contributors
 *  \file file_op.h
 *  \brief dir or file op
 */
#ifndef OPENMI_BASE_FILE_OP_H_
#define OPENMI_BASE_FILE_OP_H_

#include <dirent.h> // rmdir,mkdir
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  // access

namespace openmi {

class FileOp {
public:
  /*! \brief check whether dir_has exist */
  static bool access_dir(const char* path) {
    if (!path) return -1;
    return access(path, 0) == 0;
  }

  /*! \brief mdkir a dir. success: 0 */
  static bool mk_dir(const char* path) {
    if (!path) return -1;
    return mkdir(path, 0777) == 0;
  };

  /*! \brief delete a dir */
  static bool rm_dir(const char* path) {
    if (!path) return -1;
    return rmdir(path) == 0;
  }
}; // class FileOp

} // namespace
#endif // OPENMI_BASE_FILE_OP_H_
