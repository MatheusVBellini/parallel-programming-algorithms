#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int np, myrank;
  int token = 0;

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == 0) {
    printf("Starting Token-Ring with %d nodes...\n\n", np);
    printf("Node %d received token with value %d\n", myrank, token);

    token++;
    MPI_Send(&token, 1, MPI_INT, (token % np), (token % np), MPI_COMM_WORLD); // send to next node

    MPI_Recv(&token, 1, MPI_INT, np-1, 0, MPI_COMM_WORLD, &status); // receive message
    printf("\nToken-Ring finished!\n");

  } else {

    MPI_Recv(&token, 1, MPI_INT, myrank-1, myrank, MPI_COMM_WORLD, &status); // receive message
    printf("Node %d received token with value %d\n", myrank, token);

    token++;
    MPI_Send(&token, 1, MPI_INT, token % np, token % np, MPI_COMM_WORLD); // send to next node

  }

  fflush(0);
  if (MPI_Finalize() != MPI_SUCCESS)
    return 1;

  return 0;
}
