#!/bin/bash
cmake -U CMakeLists.txt
make

echo "Without valgrind"

for var in $(ls bin)
do
echo "Run test $var" 
echo $(./bin/$var)
done

echo "Done"