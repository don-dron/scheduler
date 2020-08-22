#!/bin/bash
cmake -U CMakeLists.txt
make clean
make

mkdir -p log

while :
do
    for var in $(ls bin)
    do
        rm -f log/log_$var.txt
        ./bin/$var >> log/log_$var.txt
    done
done