#!/bin/bash
for var in $(ls bin)
do
    rm -f log/log_$var.txt
    ./bin/$var >> log/log_$var.txt
done