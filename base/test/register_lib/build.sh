#!/bin/bash

g++ -std=c++11 -I../.. -I. ../../log_base.cc ../../log_stream.cc ../../logging.cc *.cc -o xx
