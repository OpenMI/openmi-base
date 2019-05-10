#!/bin/bash

set -o pipefail
set -o errexit

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/.. 
THIRD_PARTY_DIR=$SCRIPT_DIR/third_party
mkdir -p $THIRD_PARTY_DIR || echo "$THIRD_PARTY_DIR exists"

source $SCRIPT_DIR/deps_env.conf
mkdir -p $deps_base_path || echo "$deps_base_path exists"
mkdir -p $deps_bin_path || echo "$deps_bin_path exists"
mkdir -p $deps_include_path || echo "$deps_include_path exists"
mkdir -p $deps_lib_path || echo "$deps_lib_path exists"

################################## 
# deps 
function install_gflags() { 
  cd $THIRD_PARTY_DIR 
  rm -rf gflags || true
  url=https://github.com/gflags/gflags.git 
  git clone $url
  mkdir -p gflags/build_gflags || echo "build_gflags exists"
  cd gflags/build_gflags  
  install_dir=$THIRD_PARTY_DIR/deps/gflags 
  mkdir -p $install_dir || echo "$install_dir exists"
  export CXXFLAGS="-g -O3 -fPIC" && cmake .. -DCMAKE_INSTALL_PREFIX=$install_dir && make VERBOSE=1 -j8 && make install 

  cp -r $install_dir/include/* $deps_include_path || true
  cp -r $install_dir/lib/* $deps_lib_path || true
} 

################################## 
# 1. gflags
install_gflags

  
echo "======== ${BASH_SOURCE[0]} ========"
