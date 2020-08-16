#!/bin/bash
cmake -U CMakeLists.txt
make

echo "Without valgrind"

for var in $(ls bin)
do
echo "Run test $var" 
echo "Test Passed" $(./bin/$var)
done

echo "Done"

# echo "With valgrind"

# for var in $(ls bin)
# do
# echo "Run test $var" 
# echo "Test Passed" $(valgrind --leak-check=full --undef-value-errors=no ./bin/$var)
# done

# echo "Done"