#!/bin/sh

# id of cellular automaton 0-255
# some of the interesting are: 30, 90, 110
id=30

# width of the line
width=10000

# number of steps to execute
steps=10000

# number of processes to start if running parallel version
nprocs=2

# initialization pattern; 
# extendable through Python in capattern.py
pattern=random

#parallel environment to use
mpimod=openmpi/1.3-gcc-4.2
