#!/bin/bash

gperftools_lib_path=$HOME/.openmi_deps/lib
#gperftools_lib_path=$HOME/myhome/openmit/tech-stacks/tools_gperftools/third_party/deps/gperftools-2.7/lib

  #-L$gperftools_lib_path -ltcmalloc_minimal \
g++ -std=c++11 \
  -I.. \
  -I../.. \
  ../allocator_registry.cc \
  ../allocator.cc \
  ../logging.cc \
  allocator_test.cc \
  -o xx
