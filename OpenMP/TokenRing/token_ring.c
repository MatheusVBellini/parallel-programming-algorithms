#include <omp.h>
#include <stdio.h>

#define NODENUM 4

void node_task(int id) { printf("Node %d reached successfully.\n", id); }

int main() {

  int id[NODENUM];

#pragma omp parallel
  {
// fill up id vector
#pragma omp for
    for (int i = 0; i < NODENUM; i++) {
      id[i] = i;
    }

// start ring
#pragma omp single
    {
#pragma omp task depend(out : id[0])
      { node_task(id[0]); }

      for (int i = 1; i < NODENUM; i++) {
#pragma omp task depend(in : id[i - 1]) depend(out : id[i])
        { node_task(id[i]); }
      }
    }
  }

  return 0;
}
