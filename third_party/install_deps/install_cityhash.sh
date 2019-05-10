#!/bin/bash -e

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
function install_cityhash() { 
  cd $THIRD_PARTY_DIR 
  rm -rf cityhash
  url=https://github.com/google/cityhash.git
  git clone $url 
  cd cityhash 
  install_dir=$deps_base_path
  mkdir -p $install_dir || true
  ./configure --enable-sse4.2 CXXFLAGS="-g -O3 -fPIC" --prefix=$install_dir 
  make all check
  make install 
} 

################################## 
# 1. CityHash
install_cityhash

echo "======== ${BASH_SOURCE[0]} ========"
