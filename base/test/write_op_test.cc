#include "write_op.h"

int main(int argc, char** argv) {
  const char* file = "test_write_op_example.txt";
  openmi::Write* write = new openmi::Write(file);

  const char* log_line = "I love this world!!!";

  for (int i = 0; i < 5; ++i) {
    printf("write_line %d\n", i);
    write->write_line(log_line);
  }
  printf("flush before\n");
  write->flush();
  printf("flush after\n");

  for (int i = 0; i < 5; ++i) {
    printf("write_line 1-%d\n", i);
    write->write_line(log_line);
  }
  printf("flush before 1\n");
  write->flush();
  printf("flush after 1\n");
  
  printf("append file\n");
  
  write = new openmi::Write(file, false, true);
  log_line = "append";
  for (int i = 0; i < 5; ++i) {
    write->write_line(log_line);
  }

  write->close();

  return 0;
}
