#!/bin/bash

#=================================#
# 1. glog-0.3.5
#=================================#

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
function install_glog() {
  cd $THIRD_PARTY_DIR 
  glog_version=0.3.5
  url=https://github.com/google/glog/archive/v${glog_version}.tar.gz
  wget $url
  tar -zxvf v${glog_version}.tar.gz && cd glog-$glog_version

  install_dir=$THIRD_PARTY_DIR/deps/glog-$glog_version && (mkdir -p $install_dir || true)
  ./configure CFLAGS="-g -O3 -fPIC" CXXFLAGS="-g -O3 -fPIC" --prefix=$install_dir 
  make -j8  && make install 

  if [ $? -eq 0 ]; then
    echo "successful to build glog. drop raw package."
    rm $THIRD_PARTY_DIR/v${glog_version}.tar.gz  
    rm -rf $THIRD_PARTY_DIR/glog-${glog_version}
  else
    echo "failed to build glog.."
  fi  

  cp -r $install_dir/include/* $deps_include_path
  cp -r $install_dir/lib/*.a $deps_lib_path
}

##################################
# 1. Glog 
install_glog

echo "======== ${BASH_SOURCE[0]} ========"
