#!/bin/bash

mkdir build
cd build
cmake -DMTLIB_PATH=".." -DCMAKE_BUILD_TYPE=Debug -DQUAD=yes -DOPENCV_PATH=".." ../
make -j 2


