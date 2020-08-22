#!/bin/bash
cmake -U CMakeLists.txt
make clean
make

echo "Without valgrind"
while :
do
for var in $(ls bin)
do
echo "Run test $var" 
echo $(./bin/$var)
done

echo "Done"

done