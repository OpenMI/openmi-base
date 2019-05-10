#!/bin/bash

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
function install_gtest() {
    cd $THIRD_PARTY_DIR
    url=https://github.com/google/googletest.git
    git clone $url

    if [ ! -d googletest/build ]; then
        mkdir -p googletest/build 
    fi
    cd googletest/build 
    install_dir=$deps_base_path
    cmake -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_C_COMPILER=`which gcc` -DCMAKE_CXX_COMPILER=`which g++` ..
    make -j8 && make install
}

##################################
# 1. gtest
install_gtest

echo "======== ${BASH_SOURCE[0]} ========"
