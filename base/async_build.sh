#!/bin/bash
set -x 

exec_name=exec_async_logging

g++ -std=c++11 -pthread -g \
  logging.cc async_logging_test.cc \
  -o $exec_name 
