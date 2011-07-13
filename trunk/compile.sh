#!/bin/sh

rm -f ca
rm -f libca.so
rm -f libca-parallel.so
rm -f libca-serial.so

mpi_include_path=/usr/lib/openmpi/include/
mpi_lib_path=/usr/lib
python_include_path=/usr/include/python2.6
python_lib=python2.6

# create shared library: libca-parallel.so
gcc -fPIC -g -Wall \
    -I $mpi_include_path -lmpi -L $mpi_lib_path \
    -I $python_include_path -l$python_lib \
    -shared -Wl,-soname,libca.so \
    -o libca-parallel.so \
    capattern.c caparallel.c

# create shared library: libca-serial.so
gcc -fPIC -g -Wall \
    -I $python_include_path -l$python_lib \
    -shared -Wl,-soname,libca.so \
    -o libca-serial.so \
    capattern.c caserial.c 

# create executable: ca
ln -s libca-parallel.so libca.so
gcc main.c -o ca -lca -L./

