#!/bin/bash

set -e 

for file in $(find src/ include/ -name *.[h,c])
do 
	clang-format -i $file
done
