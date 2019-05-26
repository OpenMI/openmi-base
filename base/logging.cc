#include "logging.h"
#include <assert.h>
#include <execinfo.h>   // backtrace
#include <stdio.h>      // fread, 
#include <stdlib.h>     // popen, pclose
#include <string.h>     // strrchr
#include <sys/time.h>   // gettimeofday  
#include <unistd.h>     // access
#include <string>

using namespace openmi::internal;

namespace openmi {

ThreadLocal<internal::DateInfo>::pThreadLocal g_date = new ThreadLocal<internal::DateInfo>();
std::string g_log_dir = "";
char* g_program_name = nullptr;
char* g_log_destination_file_name = nullptr;
bool g_logtostderr = true;
bool g_alsologtostderr = false;
bool g_async = false;
int g_log_severity = INFO;

const char* log_severity_names[NUM_SEVERITIES] = {"TRACE","DEBUG","INFO","WARN","ERROR","FATAL"};
const char severity_tags[NUM_SEVERITIES] = {'T','D','I','W','E','F'}; 

const char* g_log_file_severity_names[LOG_FILE_SEVERITIES] = {"INFO", "WARN", "ERROR"};

static void WriteToStderr(const char* message, size_t len, LogSeverity severity = INFO) {
  fwrite(message, len, 1, stderr);
}

/*! \brief check whether dir_has exist */
static bool access_dir(const char* path) {
  if (!path) return false;
  return access(path, 0) == 0;
}

/*! \brief mdkir a dir. success: 0 */
static bool mk_dir(const char* path) {
  if (!path) return false;
  return mkdir(path, 0777) == 0;
};


const size_t LogMessage::kMaxLogMessageLen = 1024*256;
static std::mutex mutex; 

LogDestination* LogDestination::log_destinations_[LOG_FILE_SEVERITIES];

LogDestination* LogDestination::log_destination(int log_file_index, off_t roll_size) {
  assert(log_file_index >= 0);
  if (!log_destinations_[log_file_index]) {
    log_destinations_[log_file_index] = new LogDestination(roll_size, log_file_index);
  }
  return log_destinations_[log_file_index];
}

void LogDestination::DeleteLogDestinations() {
  for (int i = 0; i < LOG_FILE_SEVERITIES; ++i) {
    if (log_destinations_[i] != nullptr) {
      delete log_destinations_[i];
      log_destinations_[i] = nullptr;
    }
  }
}

LogDestination::LogDestination(off_t roll_size, int log_file_index) 
  : roll_size_(roll_size) { 
    log_file_.reset(
        new internal::LogFile(
          g_log_dir.c_str(), 
          g_program_name, roll_size, 
          std::string(g_log_file_severity_names[log_file_index]),
          true)  // thread safe
        );
}

void LogDestination::Write(const char* msg, size_t size) {
  log_file_->Write(msg, size);
}

void LogDestination::Flush() {
  log_file_->Flush();
}

std::string LogDestination::DebugString() {
  return "basename:" + basename_ + ", roll_size:" + std::to_string(roll_size_);
}

/*!
 * \brief log message data 
 */
struct LogMessage::LogMessageData {
  LogMessageData();

  // buffer space which contains complete message text 
  char message_text_[LogMessage::kMaxLogMessageLen+1];
  LogStream stream_;
  LogSeverity log_severity_;
  int line_;

  size_t num_chars_to_log_;

  const char* basename_;
  const char* fullname_;
  bool first_fatal_;          // true => this was first fatal msg
private:
  LogMessageData(const LogMessageData&);
  void operator=(const LogMessageData&);
}; // struct LogMessageData 

LogMessage::LogMessageData::LogMessageData(): stream_(message_text_, LogMessage::kMaxLogMessageLen) {

}

std::ostream& LogPrettyDate(internal::DateInfo& d, std::ostream& stream) {
  internal::Date::HumanDate(d);
  char pretty_date_[22];
  snprintf(pretty_date_, sizeof(pretty_date_), "%02d%02d %02d:%02d:%02d.%06d", 
           d.month, d.day, d.hour, d.minute, d.second, d.usecond);
  stream << pretty_date_;
  return stream;
}

const char* ConstShortFileName(const char* filepath) {
  // only unix-like os 
  const char* basename = strrchr(filepath, '/'); 
  return basename ? (basename+1) : filepath;
}

SendMethod send_method_ = WriteToStderr;

void LogMessage::SetSendMethod(SendMethod send_method) {
  send_method_ = send_method;
}

LogMessage::LogMessage(const char* file, int line) 
: allocated_(nullptr), data_(nullptr), log_severity_(INFO) {
  Init(file, line, log_severity_);
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
: allocated_(nullptr), data_(nullptr), log_severity_(severity) {
  Init(file, line, severity);
}

LogMessage::~LogMessage() {
  Flush();
  if (allocated_ != nullptr) {
    delete allocated_;
    allocated_ = nullptr;
  }
}

std::ostream& LogMessage::Stream() {
  if (data_ == nullptr) {
    std::string err_msg("data_ is nullptr. It maybe LogMessage::Init has not been execuated. ");
    std::string func(__func__);
    std::string file(__FILE__);
    err_msg += "func: " + func + ", file: " + file + ", line: " + std::to_string(__LINE__);
    throw std::runtime_error(err_msg);
  };
  return data_->stream_; 
} 

void LogMessage::Init(const char* file, int line, LogSeverity severity) {
  allocated_ = new LogMessageData();
  data_ = allocated_;
  
  data_->fullname_ = file;
  data_->basename_ = ConstShortFileName(data_->fullname_);
  data_->log_severity_ = severity;
  data_->first_fatal_ = false;

  Stream() << severity_tags[data_->log_severity_];
  LogPrettyDate(g_date->Value(), Stream()) 
    << ' ' << SystemInfo::GetTID() << ' ' << data_->basename_ << ':' << line << "]";
}

void LogMessage::Flush() {
  data_->num_chars_to_log_ = data_->stream_.pcount();
  bool append_newline = (data_->message_text_[data_->num_chars_to_log_-1] != '\n');
  char original_final_char = '\0';

  if (append_newline) {
    original_final_char = data_->message_text_[data_->num_chars_to_log_];
    data_->message_text_[data_->num_chars_to_log_++] = '\n';
  }

  {
    std::unique_lock<std::mutex> lock(mutex);

    if (g_async) {
      send_method_(data_->message_text_, data_->num_chars_to_log_, log_severity_);
    } else {
      if (!g_log_dir.empty() && g_program_name != nullptr) {
        int log_index = GetLogFileSeverityIndex(log_severity_);
        for (int index = 0; index <= log_index; ++index) {
          LogDestination* log_dest = LogDestination::log_destination(index);
          assert(log_dest != nullptr);
          log_dest->Write(data_->message_text_, data_->num_chars_to_log_);
          log_dest->Flush();
        }
      }
    
      if (g_log_dir.empty() || (!g_log_dir.empty() && g_alsologtostderr)) {
        send_method_(data_->message_text_, data_->num_chars_to_log_, INFO);
      }
    }
  }

  if (append_newline) {
    data_->message_text_[data_->num_chars_to_log_ - 1] = original_final_char;
  }

  if (data_->num_chars_to_log_ > kMaxLogMessageLen - 10) {
    internal::DateInfo d = g_date->Value();
    internal::Date::HumanDate(d);
    char line[100];
    snprintf(line, sizeof(line), 
             "......\nW%02d%02d %02d:%02d:%02d WARNING (%s:%d) data exceed out buffer range.\n", 
             d.month, d.day, d.hour, d.minute, d.second, ConstShortFileName(__FILE__), __LINE__);
    send_method_(line, strlen(line), log_severity_);
  }
}

// LogMessageFatal
LogMessageFatal::LogMessageFatal(const char* file, int line) : LogMessage(file, line, FATAL) {
}

LogMessageFatal::~LogMessageFatal() {
  const int MAX_STACK_SIZE = 1024;
  void* stack[MAX_STACK_SIZE];

  int nframes = backtrace(stack, MAX_STACK_SIZE);
  std::ostringstream oss;
  oss << "Stack Trace returned " << nframes << " entries:\n";
  char** msgs = backtrace_symbols(stack, nframes);
  if (msgs != nullptr) {
    for (int i = 0; i < nframes; ++i) {
      oss << "[BackTrace] (" << i << ") " << msgs[i] << "\n";
    }
  }
  throw std::runtime_error(oss.str());
} 

namespace internal {

void Date::HumanDate(DateInfo& date) {
  struct timeval tv;
  memset(&tv, 0, sizeof(timeval));
  gettimeofday(&tv, NULL);

  int sec = 0;
  sec = tv.tv_sec;
  // TODO timezone fetched by machine
  sec = sec + 3600*BJTIME;
  date.usecond = tv.tv_usec;

  if (sec % DAY == 0 || date.year == 0) {
    printf("sec // DAY: %d, date.year: %d\n", sec % DAY, date.year);
    ComputeYearMonthDay(sec, date);
  }

  if (sec % 3600 == 0 || date.hour == -1) {
    ComputeHour(sec, date);
  }

  if (sec % 60 == 0 || date.minute == -1) {
    ComputeMinute(sec, date);
  }

  ComputeSecond(sec, date);
}

void Date::ComputeYearMonthDay(int sec, DateInfo& date) {
  printf("ComputeYearMonthDay\n");
  int m[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int ad = sec / DAY;
  ad = ad - YEARSTART;
  
  int y400 = ad / YEAR400;
  int y100 = (ad - y400 * YEAR400) / YEAR100;
  int y004 = (ad - y400 * YEAR400 - y100 * YEAR100) / YEAR004;
  int y001 = (ad - y400 * YEAR400 - y100 * YEAR100 - y004 * YEAR004) / YEAR001;

  int yy = y400*4*100 + y100*100 + y004*4 + y001*1 + YEARFIRST;
  date.year = yy;
    
  int dd = (ad - y400 * YEAR400 - y100 * YEAR100 - y004 * YEAR004) % YEAR001;
  // year 
  if (yy % 1000 == 0) {
    if ((yy/1000) % 4 == 0) {
      m[1] = 29;
    }
  } else {
    if (yy % 4 == 0) {
      m[1] = 29;
    }
  }

  // month and day
  int i;
  for (i = 1; i < 12; i++) {
    if (dd - m[i] < 0) {
      break;
    } else {
      dd = dd - m[i];
    }
  }
  date.month = i;
  date.day = dd + 1;
}

void Date::ComputeHour(int sec, DateInfo& date) {
  date.hour = sec / 3600 % 24;
}

void Date::ComputeMinute(int sec, DateInfo& date) {
  date.minute = sec / 60 - sec / 3600 * 60;
}

void Date::ComputeSecond(int sec, DateInfo& date) {
  date.second = (sec - sec / 3600 * 3600) % 60;
}

// System Info

// Operator System Name
std::string SystemInfo::OsName() {
  FILE* rstream = popen("uname", "r");
  char res[100];
  fread(res, sizeof(char), sizeof(res), rstream);
  pclose(rstream);
  std::string os_name(res);
  os_name.pop_back();  // drop "\n" in os_name
  return os_name; 
}

/*!
 * Hostname
 */
std::string SystemInfo::Hostname() {
  if (host_name_.empty()) {
    printf("host_name_ is empty. get from system\n");
    char hostname[32];
    if (gethostname(hostname,sizeof(hostname)) != 0) {
      std::runtime_error("get host name error.");
      return NULL;
    }
    std::string host_name(hostname);
    //host_name.pop_back();  // drop '\n' in hostname
    host_name_ = host_name;
  }

  return host_name_;
}

pid_t SystemInfo::GetTID() {
  pid_t tid_ = 0;
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
  tid_ = (pid_t)(uintptr_t)pthread_self();
  return tid_;
#endif
}

LogFile::LogFile(const char* base_filename, 
                 const char* bin_name, 
                 off_t roll_size,
                 std::string log_severity, 
                 bool thread_safe, 
                 int flush_interval, 
                 int check_every_n) : 
  base_filename_(base_filename), 
  bin_name_(bin_name),
  roll_size_(roll_size), 
  log_severity_(log_severity), 
  start_of_period_(0), 
  last_roll_(0), 
  last_flush_(0) {
    symlink_name_ = bin_name_ + "." + log_severity_;
    first_link_ = true;

    if (!access_dir(base_filename_.c_str())) {
      printf("directory '%s' not exist. mkdir it.\n", base_filename_.c_str());
      if (!mk_dir(base_filename_.c_str())) {
        std::runtime_error("mkdir failed");
      }
    }

    if ((dp_ = opendir(base_filename_.c_str())) == NULL) {
      std::runtime_error("opendir failed");
    }

    if ((dir_fd_ = dirfd(dp_)) < 0) {
      std::runtime_error("dirfd error");
    }

    RollFile();
}

LogFile::~LogFile() { 
  Flush(); 
  close(dir_fd_);
}

void LogFile::Flush() {
  if (thread_safe_) {
    std::lock_guard<std::mutex> lock(mutex_);
    file_->flush();
  } else {
    file_->flush();
  }
}

void LogFile::Write(const char* msg, size_t size) {
  if (thread_safe_) {
    std::lock_guard<std::mutex> lock(mutex_);
    WriteUnlocked(msg, size);
  } else {
    WriteUnlocked(msg, size);
  }
}

void LogFile::WriteUnlocked(const char* msg, size_t size) {
  file_->write(msg, static_cast<int>(size));
  if (file_->written_bytes() > roll_size_) {
    RollFile();
  } else {
    count_++;
    if (count_ >= check_every_n_) {
      count_ = 0;
      time_t now = ::time(NULL);
      time_t this_period = now / roll_per_seconds * roll_per_seconds;
      if (this_period != start_of_period_) {
        RollFile();
      } else if (now - last_flush_ > flush_interval_) {
        last_flush_ = now;
        file_->flush();
      }
    }
  }
}

bool LogFile::RollFile() {
  time_t now = 0;
  std::string cur_log_filename = PrettyLogFileName(&now);
  time_t start = now / roll_per_seconds * roll_per_seconds;
    
  if (now > last_roll_) {
    last_roll_ = now;
    last_flush_ = now;
    start_of_period_ = start;
    printf("roll file reset. current log file name: %s\n", cur_log_filename.c_str());

    std::string cur_log_fullname = base_filename_ + "/" + cur_log_filename;
    file_.reset(new WriteOp(cur_log_fullname.c_str(), false, true)); 

    // file head info 
    char line[41];
    internal::DateInfo d = g_date->Value();
    internal::Date::HumanDate(d);
    snprintf(line, sizeof(line), 
             "Log file created at: %04d/%02d/%02d %02d:%02d:%02d", 
             d.year, d.month, d.day, d.hour, d.minute, d.second);
    file_->write(line, sizeof(line), true);
    
    const char* line2 = "Running on machine: ";
    file_->write(line2, strlen(line2));
    std::string machine = SystemInfo::Instance().Hostname();
    file_->write(machine.c_str(), machine.size(), true);

    const char* line3 = "Log line format: [IWEF]mmdd hh:mm:ss.uuuuuu threadid file:line] msg";
    file_->write(line3, strlen(line3), true);
    
    std::string unlinked_fullname = base_filename_ + "/" + symlink_name_;
    unlink(unlinked_fullname.c_str());
    
    if (symlinkat(cur_log_filename.c_str(), dir_fd_, symlink_name_.c_str()) < 0) {
      printf("Error: symlink %s to %s failed\n", 
          cur_log_filename.c_str(), symlink_name_.c_str());
      std::runtime_error("symlink failed.");
    } 
    if (first_link_) first_link_ = false;
    current_filename_ = cur_log_filename;
    return true;
  }
  return false; 
}

// pretty log file name
std::string LogFile::PrettyLogFileName(time_t* now) {
  std::string cur_filename = bin_name_ + "." + SystemInfo::Instance().Hostname() + ".log." + log_severity_;
  // TODO username 

  char timebuf[32];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm);
  strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S", &tm);

  cur_filename += timebuf;

  return cur_filename;
}

// class WriteOp
WriteOp::WriteOp(const char* file, bool is_binary, bool is_append): written_bytes_(0) {
  is_binary_ = is_binary;
  std::ios_base::openmode mode = 
    is_append ? std::ios_base::app : std::ios_base::trunc;
  mode_ = is_binary_ ? (mode | std::ios_base::binary) : mode;
  open(file);
}

bool WriteOp::open(const char* file) {
  ofs_.open(file, mode_);
  return ofs_.is_open() && ofs_.good();
}

bool WriteOp::write_line(const char* msg, const int length) {
  // CHECK 
  if (msg == NULL || length == 0) {
    return true;
  }
  if (is_binary_) {
    return false;
  }
  return write(msg, length, true);
}

// write text
bool WriteOp::write(const char* msg, const int length, bool endln) {
  if (msg == NULL || length == 0) {
    return true;
  }
  if (is_binary_) {
    return false;
  }
  if (ofs_.good()) {
    if (length > 0) {
      ofs_.write(msg, length);
      written_bytes_ += length;
    } else {
      ofs_ << msg;
      written_bytes_ += strlen(msg);
    }

    if (endln) {
      ofs_ << std::endl;
      written_bytes_ += 1;
    }
    return true;
  }
  return false;
}

bool WriteOp::write_binary(const char* content, size_t size) {
  if (!is_binary_) {
    return false;
  }
  ofs_.write(content, size);
  written_bytes_ += size;
  return true;
}

} // namespace internal

void InitLogging(char** argv) {
  // only unix-like os
  char* program = strrchr(argv[0], '/'); 
  g_program_name = const_cast<char*>(program ? program + 1 : argv[0]);
}

void ShutdownLogging() {
  LogDestination::DeleteLogDestinations();
  if (g_date != nullptr) {
    delete g_date; g_date = nullptr;
  }
}

} // namespace openmi
