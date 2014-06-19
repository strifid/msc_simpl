#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DQUAD=yes ../
make VERBOSE=1 -j 2


