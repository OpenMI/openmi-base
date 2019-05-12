#!/bin/bash

set -o pipefail
set -o errexit

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)
PROJECT_DIR=$SCRIPT_DIR

deps_base_path=$HOME/.openmi_deps
exec=""

while getopts "e:" opt; do
  case "$opt" in 
    e) exec=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG";;
  esac
done 

valgrind=`which valgrind`

$valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=leak.log ${exec} 

echo "$0 done"
