#!/bin/bash -x

#=================================#
# 1. valgrind-3.9.0
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
function install_valgrind() { 
  export LD_LIBRARY_PATH=$deps_lib_path:$LD_LIBRARY_PATH
  export LIBRARY_PATH=$deps_lib_path:$LIBRARY_PATH
  export C_INCLUDE_PATH=$deps_include_path:$C_INCLUDE_PATH
  export CPLUS_INCLUDE_PATH=$deps_include_path:$CPLUS_INCLUDE_PATH
  cd $THIRD_PARTY_DIR
  version=3.9.0 
  url=http://valgrind.org/downloads/valgrind-${version}.tar.bz2
  #wget $url 
  #tar xvf valgrind-${version}.tar.bz2
  cd valgrind-$version 
  install_dir=$deps_base_path
  ./configure CXXFLAGS="-g -O3 -fPIC" --prefix=$install_dir 
  make -j8 && make install 

  if [ $? -eq 0 ]; then 
    echo "successful to build valgrind. drop raw package."
    #rm $THIRD_PARTY_DIR/valgrind-${version}.tar.bz2 
    rm $THIRD_PARTY_DIR/valgrind-$version
  else
    echo "failed to build gperftools"
  fi
}

#################################
install_valgrind 

echo "======== ${BASH_SOURCE[0]} ========"
