/*!
 *  Copyright (c) 2016 by Contributors
 *  \file system_info.h
 *  \brief get system info var 
 */
#ifndef OPENMI_BASE_SYSTEM_INFO_H_ 
#define OPENMI_BASE_SYSTEM_INFO_H_ 

#include <stdlib.h> // popen, pclose
#include <stdio.h>  // fread,
#include <string>
#include <string.h>     // strrchr
#include <unistd.h>
#include <thread>

namespace openmi {

class SystemInfo {
public:
  static SystemInfo& Instance() {
    static SystemInfo sys_info;
    return sys_info;
  }
  
  std::string OsName();
  std::string Hostname();
  static pthread_t GetTID();

private:
  std::string os_;
  std::string host_name_;
}; // class SystemInfo

// Operator System Name
inline std::string SystemInfo::OsName() {
  FILE* rstream = popen("uname", "r");
  char res[100];
  fread(res, sizeof(char), sizeof(res), rstream);
  pclose(rstream);
  std::string os_name(res);
  os_name.pop_back();  // drop "\n" in os_name
  return os_name; 
}

inline const char* ConstShortFileName(const char* filepath) {
  // only unix-like os 
  const char* basename = strrchr(filepath, '/'); 
  return basename ? (basename+1) : filepath;
}

/*!
 * Hostname
 */
inline std::string SystemInfo::Hostname() {
  if (host_name_.empty()) {
    const int length = 128;
    char hostname[length];
    if (gethostname(hostname,sizeof(hostname)) != 0) {
      printf("[%s:%d] get hostname failed.\n", ConstShortFileName(__FILE__), __LINE__);
      hostname[length - 1] = '\0';
    }
    std::string host_name(hostname);
    //host_name.pop_back();  // drop '\n' in hostname
    host_name_ = host_name;
  }

  return host_name_;
}

inline pthread_t SystemInfo::GetTID() {
  pthread_t tid_ = 0;
  /*
  if (tid_ != 0) {
    return tid_;
  }
  */
  // On Linux and MacOSX, we try to use gettid().
#if defined OS_LINUX || defined OS_MACOSX
#ifndef __NR_gettid
#ifdef OS_MACOSX
#define __NR_gettid SYS_gettid
#elif ! defined __i386__
#error "Must define __NR_gettid for non-x86 platforms"
#else
#define __NR_gettid 224
#endif
#endif
  static bool lacks_gettid = false;
  if (!lacks_gettid) {
    pid_t tid = syscall(__NR_gettid);
    if (tid != -1) {
      tid_ = tid;
      return tid;
    }
    // Technically, this variable has to be volatile, but there is a small
    // performance penalty in accessing volatile variables and there should
    // not be any serious adverse effect if a thread does not immediately see
    // the value change to "true".
    lacks_gettid = true;
  }
#endif  // OS_LINUX || OS_MACOSX

  // If gettid() could not be used, we use one of the following.
#if defined OS_LINUX
  tid_ = getpid();
  return tid_;  // Linux:  getpid returns thread ID when gettid is absent
#elif defined OS_WINDOWS && !defined OS_CYGWIN
  tid_ = GetCurrentThreadId();
  return tid_;
#else
  // If none of the techniques above worked, we use pthread_self().
  tid_ = (pthread_t)(uintptr_t)pthread_self();
  return tid_;
#endif
}

} // namespace
#endif // OPENMI_BASE_SYSTEM_INFO_H_
