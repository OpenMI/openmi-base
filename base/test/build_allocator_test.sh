#!/bin/bash

gperftools_lib_path=$HOME/myhome/openmit/tech-stacks/tools_gperftools/third_party/deps/gperftools-2.7/lib

g++ -std=c++11 \
  -L$gperftools_lib_path -ltcmalloc_minimal \
  -I.. \
  ../allocator_registry.cc \
  ../log_base.cc \
  ../allocator.cc \
  ../log_stream.cc \
  ../logging.cc \
  allocator_test.cc \
  -o xx
