#include <mpi.h>
#include <omp.h>
#include <stdio.h>

#define NUM_THREADS 4

int main(int argc, char *argv[]) {
  int np, myrank;
  int token = 0;

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  return 0;
}