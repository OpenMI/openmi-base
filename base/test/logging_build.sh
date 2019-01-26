#!/bin/bash

exec_name=exec_logging

g++ -std=c++11 -pthread -g \
  logging.cc logging_test.cc \
  -o $exec_name 

#./$exec_name
