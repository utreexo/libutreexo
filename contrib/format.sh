#!/bin/bash

set -e 

for file in $(find src/ include/ tests/ -name *.[h,c])
do 
	clang-format -i $file
done
