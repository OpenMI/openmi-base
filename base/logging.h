#ifndef OPENMI_BASE_LOGGING_H_
#define OPENMI_BASE_LOGGING_H_ 

#include <dirent.h>     // opendir, DIR, rmdir, mkdir
#include <fstream>
#include <memory>
#include <mutex>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <sys/stat.h>   // mdkir
#include <sys/types.h>  // off_t, opendir, mkdir
#include <thread>
#include "thread_local.h"
using namespace openmi;

namespace openmi {

namespace internal {
  struct DateInfo;
  class WriteOp;
  class LogFile;
}

extern void InitLogging(char** argv);
extern void ShutdownLogging();

//static openmi::ThreadLocal<internal::DateInfo> g_date;
extern ThreadLocal<internal::DateInfo>::pThreadLocal g_date;
extern std::string g_log_dir;
extern char* g_program_name;
extern char* g_log_destination_file_name;     // g_log_dir/g_program_name
extern bool g_logtostderr;
extern bool g_alsologtostderr;
extern bool g_async;
extern int g_log_severity;

enum LogSeverity {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LEVELS,
};

typedef void (*SendMethod)(const char* msg, size_t size, LogSeverity severity);

static const int NUM_SEVERITIES = 6;
extern const char* log_severity_names[NUM_SEVERITIES];
extern const char severity_tags[NUM_SEVERITIES]; 

static const int LOG_FILE_SEVERITIES = 3;
extern const char* g_log_file_severity_names[LOG_FILE_SEVERITIES];

typedef void (*OutputFunc)(const char* msg, int len, LogSeverity severity);
//typedef void (*FlushFunc)(LogSeverity severity);

inline int GetLogFileSeverityIndex(LogSeverity severity) {
  if (severity <= INFO) {
    return 0;
  } else if (severity <= WARN) {
    return 1;
  } else {
    return 2;
  }
}

inline LogSeverity GetLogSeverityType(std::string log_severity_name) {
  if (log_severity_name == "TRACE" || 
      log_severity_name == "DEBUG" ||
      log_severity_name == "INFO") {
    return INFO;
  } else if (log_severity_name == "WARN") {
    return WARN;
  } else {
    return ERROR;
  }
}

class LogDestination {
public:
  static LogDestination* log_destinations_[LOG_FILE_SEVERITIES];
  static LogDestination* log_destination(int log_file_index, 
                                         off_t roll_size = 1024*1024*1024);
  static void DeleteLogDestinations();

  LogDestination(off_t roll_size, int log_file_index = 0);

  ~LogDestination() {  }

  void Write(const char* msg, size_t size);

  void Flush();

  std::string DebugString();

private:
  std::string basename_;
  off_t roll_size_;
  //LogSeverity log_severity_;
  std::unique_ptr<internal::LogFile> log_file_;

  LogDestination(const LogDestination&);
  LogDestination& operator=(const LogDestination&);
}; // class LogDestination

class LogStreamBuf: public std::streambuf {
public:
  LogStreamBuf(char* buf, int len) {
    setp(buf, buf + len - 2);
  }

  void reset() { 
    setp(pbase(), epptr()); 
  }

  virtual int_type overflow(int_type ch) {
    return ch;
  }

  size_t pcount() const { return pptr() - pbase(); }
  char* pbase() const { return std::streambuf::pbase(); }
}; // class LogStreamBuf

class LogMessage {
public:
  enum {
    kNoLogPrefix = -1
  };
  
  class LogStream: public std::ostream {
  public:
    LogStream(char* buf, int len) 
      : std::ostream(NULL), streambuf_(buf, len), self_(this) {
      rdbuf(&streambuf_);
    }

    LogStream* self() const { return self_; }

    // Legacy std::streambuf methods 
    size_t pcount() const { return streambuf_.pcount(); }
    char* pbase() const { return streambuf_.pbase();  }
    char* str() const { return pbase();  }
    void reset() { streambuf_.reset();  }

  private:
    LogStream(const LogStream&);
    LogStream* operator=(const LogStream&);
    LogStreamBuf streambuf_;
    LogStream* self_;
  }; // class LogStream

public:
  //typedef void (LogMessage::*SendMethod)();

  static void SetSendMethod(SendMethod send_method); 

  LogMessage(const char* file, int line);
  
  LogMessage(const char* file, int line, LogSeverity severity);

  ~LogMessage();

  void Init(const char* file, int line, LogSeverity severity);

  void Flush();

  std::ostream& Stream();

  static const size_t kMaxLogMessageLen;

  struct LogMessageData;

private:
  LogMessageData* allocated_;
  LogMessageData* data_;
  LogSeverity log_severity_;

  LogMessage(const LogMessage&);
  void operator=(const LogMessage&);
}; // class LogMessage

class NullStream : public LogMessage::LogStream {
public:
  NullStream() : LogMessage::LogStream(message_buffer_, 0) {}
  NullStream& Stream() { return *this; }
private:
  char message_buffer_[2];
}; // class NullStream

template <class T>
inline NullStream& operator<<(NullStream& str, const T& t) { return str; }

// support stack trace
class LogMessageFatal: public LogMessage {
public:
  LogMessageFatal(const char* file, int line);

  virtual ~LogMessageFatal();

private:
  LogMessageFatal(const LogMessageFatal&);
  void operator=(const LogMessageFatal&);
}; // class LogMessageFatal

class LogCheckError {
public:
  LogCheckError(): str(nullptr) {}

  explicit LogCheckError(const std::string& _str): str(new std::string(_str)) {}

  ~LogCheckError() {
    if (str != nullptr) {
      delete str;
      str = nullptr;
    }
  }

  operator bool() { return str != nullptr; }

  std::string* str;
}; // class LogCheckError 

class LogMessageVoidify {
public:
  LogMessageVoidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) { }
}; // class LogMessageVoidify

// LOG by severity
#define LOG_TRACE openmi::LogMessage(__FILE__, __LINE__, TRACE)
#define LOG_DEBUG openmi::LogMessage(__FILE__, __LINE__, DEBUG)
#define LOG_INFO openmi::LogMessage(__FILE__, __LINE__, INFO)
#define LOG_WARN openmi::LogMessage(__FILE__, __LINE__, WARN)
#define LOG_WARNING LOG_WARN
#define LOG_ERROR openmi::LogMessage(__FILE__, __LINE__, ERROR)
#define LOG_FATAL openmi::LogMessageFatal(__FILE__, __LINE__)
#define LOG_QFATAL LOG_FATAL 

#define LOG(severity) LOG_##severity.Stream() << ' ' 
#define VLOG(x) LOG_INFO.Stream() << ' '

#if defined(NDEBUG)
#define DCHECK_IS_ON() 0
#else
#define DCHECK_IS_ON() 1
#endif 

#if DCHECK_IS_ON()
#define DLOG(severity) LOG(severity)
#else
#define DLOG(severity) true ? (void) 0 : LogMessageVoidify() & LOG(severity)
#endif


#define SIMPLE_TRACE T
#define SIMPLE_DEBUG D
#define SIMPLE_INFO I
#define SIMPLE_WARN W
#define SIMPLE_ERROR E
#define SIMPLE_FATAL F
#define SIMPLE_QFATAL Q

#define STRING(severity) #severity

#define DEFINE_CHECK_FUNC(name, op)                                   \
  template <typename T1, typename T2>                                 \
  inline LogCheckError LogCheck##name(const T1& t1, const T2& t2) {   \
    if (t1 op t2) {                                                   \
      return LogCheckError();                                         \
    }                                                                 \
    std::ostringstream os;                                            \
    /* t1 and t2 can be serialized to string */                       \
    os << " (" << t1 << " vs. " << t2 << ") ";                        \
    return LogCheckError(os.str());                                   \
  }                                                                   \
  inline LogCheckError LogCheck##name(int x, int y) {                 \
    return LogCheck##name<int, int>(x, y);                            \
  } 

#define CHECK_BINARY_OP(name, op, c1, c2)  \
  if (openmi::LogCheckError check_err = openmi::LogCheck##name(c1, c2)) \
    openmi::LogMessageFatal(__FILE__, __LINE__).Stream() << " Check failed: " << #c1 " " #op " " #c2 << *(check_err.str)

DEFINE_CHECK_FUNC(_LT, <);
DEFINE_CHECK_FUNC(_LE, <=);
DEFINE_CHECK_FUNC(_GT, >);
DEFINE_CHECK_FUNC(_GE, >=);
DEFINE_CHECK_FUNC(_EQ, ==);
DEFINE_CHECK_FUNC(_NE, !=);

#define CHECK(x)  \
  if (!(x)) \
    openmi::LogMessageFatal(__FILE__, __LINE__).Stream() << " Check failed: " #x << ' '

#define CHECK_LT(c1, c2) CHECK_BINARY_OP(_LT, <, c1, c2)
#define CHECK_LE(c1, c2) CHECK_BINARY_OP(_LE, <=, c1, c2)
#define CHECK_GT(c1, c2) CHECK_BINARY_OP(_GT, >, c1, c2)
#define CHECK_GE(c1, c2) CHECK_BINARY_OP(_GE, >=, c1, c2)
#define CHECK_EQ(c1, c2) CHECK_BINARY_OP(_EQ, ==, c1, c2)
#define CHECK_NE(c1, c2) CHECK_BINARY_OP(_NE, !=, c1, c2) 

#define CHECK_NOTNULL(x)  \
  ((x) == NULL ? openmi::LogMessageFatal(__FILE__, __LINE__).Stream() << " Check notnull: " #x << ' ', (x) : (x))

// non NDEBUG mode
#define DCHECK(x) CHECK(x)
#define DCHECK_LT(c1, c2) CHECK((c1) < (c2))
#define DCHECK_LE(c1, c2) CHECK((c1) <= (c2))
#define DCHECK_GT(c1, c2) CHECK((c1) > (c2))
#define DCHECK_GE(c1, c2) CHECK((c1) >= (c2))
#define DCHECK_EQ(c1, c2) CHECK((c1) == (c2))
#define DCHECK_NE(c1, c2) CHECK((c1) != (c2))  


namespace internal {

#define BJTIME 8
#define DAY 86400  // 60*60*24
#define YEARFIRST 2001
#define YEARSTART (365*(YEARFIRST - 1970) + 8)
#define YEAR400 164097  // (365*4*100 + (4*(100/4 - 1) + 1))
#define YEAR100 36524   // (365*100 + (100/4 - 1))
#define YEAR004 1461    // (365*4 + 1)
#define YEAR001 365 

struct DateInfo {
  DateInfo(): year(0), month(0), day(0), hour(-1), minute(-1), second(0), usecond(0) {}
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int usecond;
}; // struct Date

class Date {
public:
  static void HumanDate(DateInfo& date);

private:
  static void ComputeYearMonthDay(int sec, DateInfo& date);
  static void ComputeHour(int sec, DateInfo& date);
  static void ComputeMinute(int sec, DateInfo& date);
  static void ComputeSecond(int sec, DateInfo& date);
}; // class Date

class SystemInfo {
public:
  static SystemInfo& Instance() {
    static SystemInfo sys_info;
    return sys_info;
  }
  
  std::string OsName();
  std::string Hostname();
  static pid_t GetTID();

private:
  std::string os_;
  std::string host_name_;
  //pid_t tid_;
}; // class SystemInfo

/*! 
 * \brief File IO Write Operator 
 * Notice: not thread-safe
 */
class WriteOp {
public:
  WriteOp(const char* file, bool is_binary = false, bool is_append = false);
    
  ~WriteOp() { close(); }
    
  bool open(const char* file);
  bool write_line(const char* msg, const int length = -1);
  bool write(const char* msg, const int length = -1, bool endln = false); // write txt
  bool write_binary(const char* content, size_t size);

  inline bool is_open() { return ofs_.is_open(); }
  inline void close() { ofs_.close(); }
  inline void flush() { ofs_.flush(); }
  inline bool is_binary() const { return is_binary_; }
  inline off_t written_bytes() { return written_bytes_; }
    
private:
  std::ofstream ofs_;
  std::ios_base::openmode mode_;
  bool is_binary_;
  off_t written_bytes_;
}; // class WriteOp

class LogFile {
public:
  LogFile(const char* base_filename, 
          const char* bin_name, 
          off_t roll_size, 
          std::string log_severity = "INFO", 
          bool thread_safe = true, 
          int flush_interval = 3, 
          int check_every_n = 1024);

  ~LogFile();

  void Flush();
  
  void Write(const char* msg, size_t size);

  void WriteUnlocked(const char* msg, size_t size);

  bool RollFile();

private:
  std::string PrettyLogFileName(time_t* now);

private:
  std::string base_filename_;
  std::string bin_name_;
  off_t roll_size_;
  bool thread_safe_;
  std::string log_severity_;
  time_t start_of_period_;
  time_t last_roll_;
  time_t last_flush_;
  
  DIR *dp_;
  int dir_fd_;
  std::string symlink_name_;
  std::string current_filename_;
  bool first_link_;
  int flush_interval_;
  int check_every_n_;
  int count_;

  std::mutex mutex_;
  std::shared_ptr<internal::WriteOp> file_;

  const static int roll_per_seconds = 60*60*24;
}; // class LogFile

} // namespace internal

} // namespace openmi
#endif // OPENMI_BASE_LOGGING_H_
