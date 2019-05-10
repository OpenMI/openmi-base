#!/bin/bash -x 

#=================================#
# 1. gperftools-2.7
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
function install_gperftools() { 
  export LD_LIBRARY_PATH=$deps_lib_path:$LD_LIBRARY_PATH
  export LIBRARY_PATH=$deps_lib_path:$LIBRARY_PATH
  export C_INCLUDE_PATH=$deps_include_path:$C_INCLUDE_PATH
  export CPLUS_INCLUDE_PATH=$deps_include_path:$CPLUS_INCLUDE_PATH
  cd $THIRD_PARTY_DIR
  version=2.7
  url=https://github.com/gperftools/gperftools/archive/gperftools-${version}.tar.gz
  wget $url 
  tar -zxvf gperftools-${version}.tar.gz 
  cd gperftools-gperftools-$version
  aclocal && autoconf && autoheader && libtoolize --automake --copy --debug  --force && automake -a  
  install_dir=$deps_base_path
  ./configure CXXFLAGS="-g -O3 -fPIC" --prefix=$install_dir 
  make -j8 && make install 

  if [ $? -eq 0 ]; then 
    echo "successful to build gperftools. drop raw package"
    rm $THIRD_PARTY_DIR/gperftools-${version}.tar.gz 
    rm -rf $THIRD_PARTY_DIR/gperftools-gperftools-${version}
  else
    echo "failed to build gperftools."
  fi 
}

################################## 
# deps
# 1. gperftools
install_gperftools

echo "======== ${BASH_SOURCE[0]} ========"
