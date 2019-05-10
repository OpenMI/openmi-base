#!/bin/bash -x 

#=================================#
# 1. eigen-3.3.4
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

export LD_LIBRARY_PATH=$deps_lib_path:${LD_LIBRARY_PATH}
export LIBRARY_PATH=$deps_lib_path:${LIBRARY_PATH}
export C_INCLUDE_PATH=$deps_include_path:${C_INCLUDE_PATH}
export CPLUS_INCLUDE_PATH=$deps_include_path:${CPLUS_INCLUDE_PATH}

##################################
function install_eigen() {
  cd $THIRD_PARTY_DIR 
  version=3.3.4
  url=http://bitbucket.org/eigen/eigen/get/${version}.tar.bz2
  wget $url
  tar -xjf ${version}.tar.bz2
  cd eigen-eigen*
  install_dir=$deps_base_path
  mkdir -p build || echo "build exist!" 
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=$install_dir .. 
  make -j8 && make install 
  cd $THIRD_PARTY_DIR/deps   
  ln -s eigen-${version} eigen

  if [ $? -eq 0 ]; then
    echo "successful to build eigen-$version. drop raw package."
    rm $THIRD_PARTY_DIR/${version}.tar.bz2 || echo "rm done!"
    rm -rf $THIRD_PARTY_DIR/eigen-${version} || echo "rm done!"
  else
    echo "failed to build eigen.."
  fi
}

##################################
# 1. eigen
install_eigen

echo "======== ${BASH_SOURCE[0]} ========"
