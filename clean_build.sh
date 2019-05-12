#!/bin/bash 

set -x
set -o pipefail
set -o errexit

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)
PROJECT_DIR=$SCRIPT_DIR
cd $PROJECT_DIR 

if [ -d build ]; then 
  cd build && make clean 
  cd ..
fi

rm -rf lib bin build log || true 

echo "clear_build done"
