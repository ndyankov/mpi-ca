#include <stdio.h>
#include <stdlib.h>

#include "ca.h"

/* Initialize the calculation */
void CA_Init(int argc, char *argv[]){
}

/* Finalize the calculation */
void CA_Finalize(){
}

/* Instantiate a CA */
struct CA *CA_Create(int id, int width, char *pattern) {
    struct CA *ca = (struct CA *) malloc(sizeof(struct CA));
    ca->id = id;
    ca->width = width;
    ca->cells = CA_LoadCells(pattern, ca->width);
    if (ca->cells == NULL) {
        fprintf(stderr, "error: could not load cells\n");
        return NULL;
    }

    return ca;
}

void CA_Run(struct CA *ca, int steps) {
    CA_Sow(ca);
    // PBM header
    printf("P1\n");
    printf("# rule %d\n", ca->id);
    printf("%d %d\n", ca->width, steps);

    CA_Print(ca);
    CA_Iterate(ca, steps);
}

void CA_Sow(struct CA *ca) {
}

void CA_Iterate(struct CA *ca, int steps) {
    int step;
    for (step=0; step<steps-1; step++) {
        CA_Step(ca);
        CA_Print(ca);
    }
}

/* Execute a single step in the evolution of CA */
void CA_Step(struct CA *ca) {
    int i;
    int cc[ca->width];

    for (i=0; i<ca->width; i++) {
        cc[i] = CA_Calc(i, ca->id, ca->width, ca->cells);
    }

    // replace the original cells with the new ones
    for (i=0; i<ca->width; i++) {
        *(ca->cells+i) = cc[i];
    }
}

/* Calculate the new value of a cell */
int CA_Calc(int index, int id, int width, int *cells) {
    // the calculation is such so that the numbering scheme
    // matches the one defined in Stephen Wolfram's NKS book
    int i, r;
    r = 0;
    for (i=-1; i<=1; i++) {
        r = 2*r + *(cells+CA_Index(index+i, width));
    }

    // get the r-th bit of ca_id
    return ((1<<r) & id) >> r;
}

int CA_Index(int i, int ca_width) {
    return (i+ca_width) % ca_width;
}

void CA_Print(struct CA *ca) {
    int i;
    for (i=0; i<ca->width; i++){
       // PBM data
       printf("%s", *(ca->cells+i) ? "1 " : "0 ");
    }
    printf("\n");
}

