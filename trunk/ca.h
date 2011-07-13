#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

struct CA {
    int *cells;  // pointer to cells
    int id;      // id of the CA
    int width;   // width of the line
    int rank;    // rank of the process
    int nprocs;  // number of processes
    double time; // timing data
};

void CA_Init(int argc, char *argv[]);
void CA_Finalize();
struct CA *CA_Create(int id, int width, char *pattern);
void CA_Run(struct CA *ca, int steps);
void CA_Iterate(struct CA *ca, int steps);
void CA_Sow(struct CA *ca);
void CA_Step(struct CA *ca);
int CA_Calc(int index, int id, int width, int *cells);
int CA_GetStart(struct CA *ca);
int CA_GetEnd(struct CA *ca);
int CA_Index(int i, int ca_width);
int CA_Rank(int rank, int nprocs);
void CA_Collect(struct CA *ca);
void CA_Print(struct CA *ca);
int CA_GetStartRank(struct CA *ca, int rank);
int CA_GetEndRank(struct CA *ca, int rank);
int *CA_LoadCells(char *pattern, int width);
#endif

