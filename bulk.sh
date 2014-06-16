#!/bin/bash

CURDIR=`pwd`
cd ./bulk
IN_FILE=$1

echo $CURDIR

../build/gradient_msc -i "$CURDIR/$IN_FILE" -o ../output/test.jpg -p 0
../build/gradient_msc -i $CURDIR/$IN_FILE -o ../output/test.jpg -p 10
../build/gradient_msc -i $CURDIR/$IN_FILE -o ../output/test.jpg -p 100
#../build/gradient_msc -i $CURDIR/$IN_FILE -o ../output/test.jpg -p 300
#../build/gradient_msc -i $CURDIR/$IN_FILE -o ../output/test.jpg -p 500
#../build/gradient_msc -i $CURDIR/$IN_FILE -o ../output/test.jpg -p 1000

for file in *.txt
do
mv $file "../output/"${IN_FILE##*/}"_"$file

done




