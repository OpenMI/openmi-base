#!/bin/bash 

set -o pipefail
set -o errexit

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)
PROJECT_DIR=$SCRIPT_DIR

deps_base_path=$HOME/.openmi_deps

while getopts "b:" opt; do
  case "$opt" in 
    b) deps_base_path=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG";;
  esac
done 

export DEPS_ENV_HOME=$deps_base_path

mkdir -p $PROJECT_DIR/build || echo "$PROJECT_DIR/build exists!"
cd $PROJECT_DIR/build 

dynamic_linker=""
os_name=`uname | tr "A-Z" "a-z"` 
if [[ "$os_name" == "linux" ]]; then
  dynamic_linker="/lib64/ld-lsb-x86-64.so"
fi

#build_type="Debug"
build_type="Release"

cmake $PROJECT_DIR \
-DCMAKE_C_COMPILER=`which gcc` \
-DCMAKE_CXX_COMPILER=`which g++` \
-DCMAKE_BUILD_TYPE=$build_type \

make -j8

echo "======== ${BASH_SOURCE[0]} ========"
