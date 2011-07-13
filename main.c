#include <stdio.h>
#include <stdlib.h>

#include "ca.h"

/* Read input parameters from command line */
int readCAParams(int argc, char *argv[], int *ca_id, int *ca_width, int *steps, char **pattern) {
    // read command-line arguments
    if (argc != 5) {
        return 2;
    }

    *ca_id = atoi(argv[1]);
    *ca_width = atoi(argv[2]);
    *steps = atoi(argv[3]);
    *pattern = argv[4];
    return 0;
}

int main(int argc, char *argv[]) {
    int id;             // id of the CA
    int width;          // width of the line
    int steps;          // number of steps to execute
    char *pattern;      // name of the init pattern

    int error;
    error = readCAParams(argc, argv, &id, &width, &steps, &pattern);
    if (error) {
        fprintf(stderr, "usage: %s <id> <width> <steps> <pattern>\n", argv[0]);
        return error;
    }

    CA_Init(argc, argv);

    struct CA *ca = CA_Create(id, width, pattern);
    if (ca == NULL) {
        CA_Finalize();
        return 2;
    }

    CA_Run(ca, steps);
    CA_Finalize();
    return 0;
}

