#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DEPS_DIR=$SCRIPT_DIR/install_deps

{
echo "install eigen"
#sh $INSTALL_DEPS_DIR/install_eigen.sh &

sleep 1

echo "install gperftools"
sh $INSTALL_DEPS_DIR/install_gperftools.sh &

echo "install sparsehash"
sh $INSTALL_DEPS_DIR/install_sparsehash.sh &

echo "install cityhash"
sh $INSTALL_DEPS_DIR/install_cityhash.sh &

echo "install gtest"
sh $INSTALL_DEPS_DIR/install_gtest.sh

echo "install gflags"
sh $INSTALL_DEPS_DIR/install_gflags.sh

} 
wait

{
#echo "install glog"
#sh $INSTALL_DEPS_DIR/install_glog.sh &

echo "install protobuf"
sh $INSTALL_DEPS_DIR/install_protobuf.sh &

#echo "install boost "
#sh $INSTALL_DEPS_DIR/install_boost.sh &
} 
wait 

echo "install openssl, libevent, thrift, snappy"
#sh $INSTALL_DEPS_DIR/install_thrift_openssl_libevent_snappy.sh 

