#!/bin/bash -e

#=================================#
# 1. boost-1.60.0
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
# https://github.com/boost/boost/blob/master/INSTALL.md
function install_boost() {
  cd $THIRD_PARTY_DIR
  version=1_60_0 
  raw_version=1.60.0
  wget --no-check-certificate http://sourceforge.net/projects/boost/files/boost/${raw_version}/boost_${version}.tar.gz
  tar -zxvf boost_${version}.tar.gz 
  cd boost_${version}

  install_dir=$deps_base_path
  ./bootstrap.sh --prefix=${install_dir}
  ./b2 install

  # drop 
  if [ $? -eq 0 ]; then 
    echo "success to build boost. drop raw package."
    rm -rf $THIRD_PARTY_DIR/boost* 
  else
    echo "failed to build boost."
  fi
}

##################################
# 1. boost
install_boost

echo "======== ${BASH_SOURCE[0]} ========"
