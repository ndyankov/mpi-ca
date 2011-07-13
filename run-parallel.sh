#!/bin/sh

. ./config.sh

# create a link to the parallel shared library
rm -f libca.so
ln -s libca-parallel.so libca.so

# place the library on library path
LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

PYTHONPATH=.:$PYTHONPATH
export PYTHONPATH

for id in $(seq -w 30 30); do
    image="images/$pattern-$id-$width-$steps.pbm"
    echo $image
    mpirun -n $nprocs ca $id $width $steps $pattern > $image
done

