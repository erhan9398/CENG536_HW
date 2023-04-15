#!/bin/bash

# Check if the number of arguments is correct
if [ "$#" -ne 1 ]; then
    echo "Please specify the part of HW that you want to compile and run"
    echo "e.g. ./run.sh part1"
    exit 1
fi

# Retrieve arguments
arg1="$1"

if [ "$arg1" != "part1" ] && [ "$arg1" != "part2" ] && [ "$arg1" != "part3"]; then
    echo "please specify part number that you want to run"
    echo "e.g. ./run.sh part1"
    exit 1
fi

if [ ! -d build ]; then
    echo "there is no build dir"
    echo "creating build dir..."
    mkdir build
else
    echo "build dir already exist"
fi

if [ ! -d file ]; then
    echo "there must be file dir"
    echo "creating file dir..."
    mkdir file
fi

cd build
cmake .. -DCMAKE_ARGS=$arg1 
make -j$(nproc) 
