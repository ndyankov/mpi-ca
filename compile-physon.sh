#!/bin/bash

rm -f ca
rm -f libca.so
rm -f libca-parallel.so
rm -f libca-serial.so

# source configuration file which defines $mpimod
. ./config.sh

python_include_path=/usr/include/python2.5
python_lib=python2.5

module load $mpimod

# create shared library: libca-parallel.so
mpicc -fPIC -g -Wall \
      -I $python_include_path -l$python_lib \
      -shared -Wl,-soname,libca.so \
      -o libca-parallel.so \
      capattern.c caparallel.c

# create executable: ca
ln -s libca-parallel.so libca.so
gcc main.c -o ca -lca -L./
