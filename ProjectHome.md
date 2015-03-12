This is an educational project written for a Parallel Programming with MPI course.

It can simulate all the 256 elementary cellular automata - one-dimensional with two-states.

The program has two implementations - serial and parallel. The latter uses MPI and can be run on any cluster environment supporting it.

Main part is written in C, while Python is used for configuration of the initial conditions of CA.

Both implementations are abstracted in two shared libraries providing the same methods.