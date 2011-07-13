#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "ca.h"

/* Initialize the calculation */
void CA_Init(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
}

/* Finalize the calculation */
void CA_Finalize(){
    MPI_Finalize();
}

/* Instantiate a CA */
struct CA *CA_Create(int id, int width, char *pattern) {
    int rank, nprocs;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    struct CA *ca = (struct CA *) malloc(sizeof(struct CA));
    // initialize cells
    ca->cells = NULL;
    ca->id = id;
    ca->width = width;
    ca->rank = rank;
    ca->nprocs = nprocs;
    ca->time = 0;

    if (rank == 0) {
        // process with rank 0 loads cells
        ca->cells = CA_LoadCells(pattern, ca->width);
        if (ca->cells == NULL) {
            fprintf(stderr, "error: could not load cells\n");
            return NULL;
        }
    } else {
        // other processes initialize an empty array
        ca->cells = (int *) malloc(width * sizeof(int));
    }

    return ca;
}

/* Run the CA */
void CA_Run(struct CA *ca, int steps) {
    CA_Sow(ca);
    // PBM header
    if (ca->rank == 0) {
        printf("P1\n");
        printf("# rule %d\n", ca->id);
        printf("%d %d\n", ca->width, steps);
    }

    CA_Print(ca);

    // perform a global synchronisation
    MPI_Barrier(MPI_COMM_WORLD);

    ca->time = -MPI_Wtime();
    CA_Iterate(ca, steps);
    ca->time += MPI_Wtime();
    
    if (ca->rank == 0) {
        fprintf(stderr, "%d:%d:%d:%f\n", ca->nprocs, ca->width, steps, ca->time);
    }
}

/* Iterate the CA for a number of steps */
void CA_Iterate(struct CA *ca, int steps) {
    int step;
    for (step=0; step<steps-1; step++) {
        CA_Step(ca);
        CA_Print(ca);
    }
}

/* Setup the initial conditions of the CA */
void CA_Sow(struct CA *ca) {
    int r, start, end;

    // distribute initial conditions
    if (ca->rank == 0) {
        // distribute cells to other processes
        for (r=1; r<ca->nprocs; r++) {
            start = CA_GetStartRank(ca, r);
            end = CA_GetEndRank(ca, r);
            MPI_Send(ca->cells+start, end-start, MPI_INT, r, 0, MPI_COMM_WORLD);
        }
    } else {
        // receive cells from process with rank 0
        r = 0;
        start = CA_GetStartRank(ca, ca->rank);
        end = CA_GetEndRank(ca, ca->rank);
        MPI_Status *status = NULL;
        MPI_Recv(ca->cells+start, end-start, MPI_INT, r, 0, MPI_COMM_WORLD, status);
    }
}

/* Send cell to a process */
void CA_SendCell(struct CA *ca, int to, int index) {
    to = CA_Rank(to, ca->nprocs);
    index = CA_Index(index, ca->width);
    MPI_Send(ca->cells+index, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
}

/* Receive cell from a process */
void CA_ReceiveCell(struct CA *ca, int from, int index) {
    from = CA_Rank(from, ca->nprocs);
    index = CA_Index(index, ca->width);
    MPI_Status *status = NULL;
    MPI_Recv(ca->cells+index, 1, MPI_INT, from, 0, MPI_COMM_WORLD, status);
}

/* Exchange corner cells with neighbouring processes */
void CA_ExchangeCells(struct CA *ca) {
    int rank, nprocs;
    int start, end;

    start = CA_GetStart(ca);
    end = CA_GetEnd(ca);

    rank = ca->rank;
    nprocs = ca->nprocs;

//              start         end
//                |            |
//                v            v
//     oooooooooo|oooooooooooo|ooooooooooooo
// <-- rank-1 --> <-- rank --> <-- rank+1 -->

    if (!(nprocs % 2)) {
        // for even number of processes
        if (rank % 2) {
            // 1a receive from left
            CA_ReceiveCell(ca, rank-1, start-1);
            // 1b send to left
            CA_SendCell(ca, rank-1, start);

            // 2a receive from right
            CA_ReceiveCell(ca, rank+1, end);
            // 2b send to right
            CA_SendCell(ca, rank+1, end-1);
        } else {
            // 1a send to right
            CA_SendCell(ca, rank+1, end-1);
            // 1b receive from right
            CA_ReceiveCell(ca, rank+1, end);

            // 2a send to left
            CA_SendCell(ca, rank-1, start);
            // 2b receive from left
            CA_ReceiveCell(ca, rank-1, start-1);
        }
    } else {
        // for odd number of processes
 
        // for a single process there is nothing to exchange
        if (nprocs > 1) {
            // 1b send to left
            CA_SendCell(ca, rank-1, start);
            // 2a receive from right
            CA_ReceiveCell(ca, rank+1, end);
        }

        // for two processes left and right are the same
        if (nprocs > 2) {
            // 2b send to right
            CA_SendCell(ca, rank+1, end-1);
            // 1a receive from left
            CA_ReceiveCell(ca, rank-1, start-1);
        }
    }
}

/* Execute a single step in the evolution of CA */
void CA_Step(struct CA *ca) {
    int i;
    int cc[ca->width];

    CA_ExchangeCells(ca);

    for (i=CA_GetStart(ca); i<CA_GetEnd(ca); i++) {
        cc[i] = CA_Calc(i, ca->id, ca->width, ca->cells);
    }

    // replace the original cells with the new ones
    for (i=CA_GetStart(ca); i<CA_GetEnd(ca); i++) {
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

/* Normalize a process rank to be within range */
int CA_Rank(int rank, int nprocs) {
    return (rank+nprocs) % nprocs;
}

/* Normalize a cell index to be within range */
int CA_Index(int i, int ca_width) {
    return (i+ca_width) % ca_width;
}

/* Get the start cell of the range assigned to current process */
int CA_GetStart(struct CA *ca) {
    return (ca->width)*(ca->rank)/(ca->nprocs);
}

/* Get the end cell of the range assigned to current process */
int CA_GetEnd(struct CA *ca) {
    return (ca->width)*(ca->rank+1)/(ca->nprocs);
}

/* Get the start cell of the range assigned to process with a certain rank */
int CA_GetStartRank(struct CA *ca, int rank) {
    return (ca->width)*rank/(ca->nprocs);
}

/* Get the end cell of the range assigned to process with a certain rank */
int CA_GetEndRank(struct CA *ca, int rank) {
    return (ca->width)*(rank+1)/(ca->nprocs);
}

/* Collects pieces to process with rank zero */
void CA_Collect(struct CA *ca) {
    int rank;
    int start, end;
    MPI_Status *status = NULL;
    if (ca->rank == 0) {
        for (rank=1; rank<ca->nprocs; rank++) {
            start = CA_GetStartRank(ca, rank);
            end = CA_GetEndRank(ca, rank);
            MPI_Recv(ca->cells+start, end-start, MPI_INT, rank, 0, MPI_COMM_WORLD, status);
        }
    } else {
       start = CA_GetStart(ca);
       end = CA_GetEnd(ca);
       MPI_Send(ca->cells+start, end-start, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

void CA_Print(struct CA *ca) {
    CA_Collect(ca);
    // exclude time for printing
    ca->time += MPI_Wtime();
    if (ca->rank == 0) {
        int i;
        for (i=0; i<ca->width; i++){
            // PBM data
            printf("%s", *(ca->cells+i) ? "1 " : "0 ");
        }
        printf("\n");
    }
    ca->time -= MPI_Wtime();
}

