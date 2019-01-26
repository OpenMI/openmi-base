#include "log_file.h"

void test_log_file() {
  const char* file_name = "logfile_test.txt";
  openmi::LogFile log_file(file_name, 256);

  const char* line = "I love this world!!!!\n";
  for (int i = 0; i < 10; ++i) {
    log_file.Write(line, strlen(line));
    //sleep(1);
  }
  log_file.Flush();
}

void test_log_file_manager() {
  const char* file_name = "logfilemanager_test.txt";
  openmi::LogFileManager log_file_mgr(file_name, 1024*1024);
  const char* line = "I love this world!!!\n";
  for (int  i = 0; i < 1000; ++i) {
    log_file_mgr.Write(line, strlen(line), GetLogSeverityType(std::string(g_log_file_severity_names[i%LOG_FILE_SEVERITIES])));
  }
}

int main(int argc, char** argv) {
  test_log_file();
  test_log_file_manager();
  return 0;
}
