#!/bin/bash

set -e 

for file in $(find src/ include/ -name *.h)
do 
	clang-format -i $file
done
