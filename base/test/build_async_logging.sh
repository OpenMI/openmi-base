#!/bin/bash

g++ -std=c++11 -pthread \
  -I.. \
  async_logging_test.cc \
  ../log_stream.cc \
  ../logging.cc \
  ../log_base.cc \
  ../write_op.cc \
  -o xx
