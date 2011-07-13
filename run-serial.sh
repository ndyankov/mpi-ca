#!/bin/sh

. ./config.sh

# create a link to the serial shared library
rm -f libca.so
ln -s libca-serial.so libca.so

# place the library on library path
LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

PYTHONPATH=.:$PYTHONPATH
export PYTHONPATH

for id in $(seq -w 0 255); do
    image="images/$pattern-$id-$width-$steps.pbm"
    echo $image
    ./ca $id $width $steps $pattern > $image
    #xdg-open $image
done
