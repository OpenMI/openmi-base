#!/bin/bash

bin=xx

g++ -std=c++11 -I../.. -I. ../../logging.cc *.cc -o $bin

echo "build $bin done."
