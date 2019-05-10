#!/bin/bash -e

#=================================#
# 1. protobuf-3.5.0
#=================================#

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
function install_protobuf() {
  cd $THIRD_PARTY_DIR 
  version=3.5.0
  url="https://github.com/google/protobuf/archive/v${version}.tar.gz"
  wget $url 
  tar -zxvf v${version}.tar.gz

  install_dir=$deps_base_path
  cd protobuf-$version
  ./autogen.sh
  ./configure CXXFLAGS="-g -O3 -fPIC -msse4.2" --prefix=$install_dir --with-pic ||true
  make clean || true 
  make -j8 || true
  make install || true

  if [ $? -eq 0 ]; then
    echo "successful to build protobuf. drop raw package."
    rm $THIRD_PARTY_DIR/v$version.tar.gz  
    rm -rf $THIRD_PARTY_DIR/protobuf-$version
  else
    echo "failed to build protobuf.."
  fi  
}
##################################
# 1. protobuf 
install_protobuf

echo "======== ${BASH_SOURCE[0]} ========"
