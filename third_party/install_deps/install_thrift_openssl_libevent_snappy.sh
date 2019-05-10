#!/bin/bash -x 

#=================================#
# openssl-1.1.1
# libevent-2.0.22
# thrift-0.10.0
# snappy-1.1.7
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
function install_libevent() { 
  export LD_LIBRARY_PATH=$deps_lib_path:$LD_LIBRARY_PATH
  export LIBRARY_PATH=$deps_lib_path:$LIBRARY_PATH
  export C_INCLUDE_PATH=$deps_include_path:$C_INCLUDE_PATH
  export CPLUS_INCLUDE_PATH=$deps_include_path:$CPLUS_INCLUDE_PATH
  cd $THIRD_PARTY_DIR
  version=2.0.22
  url=https://github.com/nmathewson/Libevent/archive/release-${version}-stable.tar.gz
  wget $url 
  tar -zxvf release-${version}-stable.tar.gz  
  cd Libevent-release-${version}-stable 
  aclocal && autoconf && autoheader && libtoolize --automake --copy --debug --force && automake -a  
  autoreconf --force --install  # Avoid This is libtool 2.4.6, but the efinition of this LT_INIT comes from libtool 2.4.2
  ./configure CFLAGS=" -g -O3 -fPIC" --with-pic --disable-openssl --prefix=$deps_base_path
  make -j8 && make install 

  if [ $? -eq 0 ]; then 
    echo "successful to build libevent. drop raw package"
    rm $THIRD_PARTY_DIR/release-${version}-stable.tar.gz 
    rm -rf $THIRD_PARTY_DIR/Libevent-release-${version}-stable 
  else
    echo "failed to build libevent."
  fi 
}

function install_openssl() {
  cd $THIRD_PARTY_DIR
  version=1.1.1
  url=https://www.openssl.org/source/old/${version}/openssl-${version}-pre7.tar.gz
  wget $url
  tar -zxvf openssl-${version}-pre7.tar.gz
  cd openssl-1.1.1-pre7
  mkdir -p $deps_base_path || echo "$deps_base_path exists"
  ./config CFLAGS="-g -O3 -shared -fPIC" --prefix=$deps_base_path 
  ./config -t
  make && make install 
}

function install_thrift() {
  export LD_LIBRARY_PATH=$deps_lib_path:$LD_LIBRARY_PATH
  export LIBRARY_PATH=$deps_lib_path:$LIBRARY_PATH
  export C_INCLUDE_PATH=$deps_include_path:$C_INCLUDE_PATH
  export CPLUS_INCLUDE_PATH=$deps_include_path:$CPLUS_INCLUDE_PATH
  cd $THIRD_PARTY_DIR 
  thrift_version=0.10.0
  url=https://github.com/apache/thrift/archive/${thrift_version}.tar.gz
  wget $url
  tar -zxvf ${thrift_version}.tar.gz
  cd thrift-$thrift_version

  install_dir=$deps_base_path
  ./bootstrap.sh
  ./configure --with-boost-libdir=$deps_lib_path CXXFLAGS='-g -O3 -fPIC' CFLAGS='-g -O3' CPPFLAGS='-DDEBUG_MY_FEATURE' --enable-coverage --prefix=$install_dir --with-php=no --with-php_extension=no --with-dart=no --with-ruby=no --with-haskell=no --with-go=no --with-rs=no --with-haxe=no --with-dotnetcore=no --with-d=no --with-qt4=no --with-qt5=no --with-csharp=no --with-java=no --with-erlang=no --with-nodejs=no --with-lua=no --with-perl=no --with-python=no --with-cpp --with-libevent=$deps_base_path --with-openssl=$deps_base_path --enable-tests=no
  make -j8 && make install 

  if [ $? -eq 0 ]; then
    echo "successful to build thrift. drop raw package."
    rm $THIRD_PARTY_DIR/${thrift_version}.tar.gz* || echo "rm done!"
    rm -rf $THIRD_PARTY_DIR/thrift-${thrift_version} || echo "rm done!"
  else
    echo "failed to build thrift.."
  fi 
}

function install_snappy() {
  cd $THIRD_PARTY_DIR 
  snappy_version=1.1.7
  url=https://github.com/google/snappy/archive/${snappy_version}.tar.gz
  wget $url
  tar -zxvf ${snappy_version}.tar.gz && cd snappy-${snappy_version} 
  mkdir build || echo "build exists"
  cd build  
  install_dir=$deps_base_path
  cmake -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_C_COMPILER=`which gcc` -DCMAKE_CXX_COMPILER=`which g++` -DSNAPPY_BUILD_TESTS=0 ../
  make -j8 && make install 
}

################################## 
# deps
install_openssl

install_libevent

install_thrift

install_snappy

echo "======== ${BASH_SOURCE[0]} ========"
