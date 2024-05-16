/*
* Given a vec[size] of integers, find the index of all integers greater than vec[K]
*/

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int size = -1, K = -1, finished = 0;
  int *vec = NULL;
  int *index = NULL;
  FILE *fp; char filename[512];
  
  // argument processing
  if (argc < 2) {
    printf("Missing target file! Exiting program\n");
    return 1;
  }
  strcpy(filename, argv[1]);

  // read file
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Unable to open the file!\n");
    return 1;
  }
  fscanf(fp, "%d", &size);
  fscanf(fp, "%d", &K);

  vec = (int*)malloc(size*sizeof(int));
  index = (int*)malloc(size*sizeof(int));
  for (int i = 0; i < size; i++) {
    index[i] = -1;
    fscanf(fp, "%d", &vec[i]);
  }

  fclose(fp);

  // MPI init
  int np, myrank;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  // Process routines
  if (myrank == 0) {

    int i = 0;
    int msg_size = (1 + size/(np-1));
    int *recv = (int*)malloc(msg_size*sizeof(int));

    // collect indexes
    while (finished != np-1) {
      MPI_Recv(recv, msg_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status); // receive index
      // save received results
      for (int j = 1; j <= recv[0]; j++)
        index[i++] = recv[j];
      finished++;
    }

    printf("Numbers of values greater than %d: %d\n", vec[K], i);
    printf("Indices: [ ");
    for (int j = 0; j < i; j++) {
      printf("%d ", index[j]);
    }
    printf("]\n");
    printf("Values: [ ");
    for (int j = 0; j < i; j++) {
      printf("%d ", vec[index[j]]);
    }
    printf("]\n");

    free(recv);

  } else {
    
    int inf = (myrank-1)*(size/(np-1));
    int sup = (myrank != np-1) ? (myrank)*(size/(np-1)) : size;
    int i;

    int msg_size = (1 + size/(np-1));
    int *msg = (int*)malloc(msg_size*sizeof(int)); // { size_of_array , index_1 , ... , index_size_of_array }
    int aux_i = 1;
    msg[0] = 0;
    for (i = inf; i < sup; i++) {
      if (vec[i] > vec[K]) {
        msg[0]++;
        msg[aux_i++] = i;
      }
    }
    MPI_Send(msg, msg_size, MPI_INT, 0, 0, MPI_COMM_WORLD); // send index vector
    free(msg);
  } 

  fflush(0);
  free(vec);
  free(index);
  if (MPI_Finalize() != MPI_SUCCESS)
    return 1;

  return 0;

}
