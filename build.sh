#!/bin/bash
rm -f CMakeCache.txt

cmake -U CMakeLists.txt
make clean
make

mkdir -p log
