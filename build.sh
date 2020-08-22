#!/bin/bash
cmake -U CMakeLists.txt
make clean
make

mkdir -p log
