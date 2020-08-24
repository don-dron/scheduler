#!/bin/bash

# count=0
# all=$(find bin -type f| wc -l)

for var in $(ls bin)
do  
    # ((count++))
    rm -f log/$var.txt
    ./bin/$var >> log/$var.txt
    # echo $count/$all $var
done