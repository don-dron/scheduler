#!/bin/bash
rm -f CMakeCache.txt

cmake -U CMakeLists.txt
cmake --build .

mkdir -p log
