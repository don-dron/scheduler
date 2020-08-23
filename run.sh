#!/bin/bash

# count=0
# all=$(find bin -type f| wc -l)

for var in $(ls bin)
do  
    # ((count++))
    rm -f log/log_$var.txt
    ./bin/$var >> log/log_$var.txt
    # echo $count/$all $var
done