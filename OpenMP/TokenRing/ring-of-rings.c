#include <omp.h>
#include <stdio.h>

#define NODENUM 4
#define SUBNODENUM 4

int id[NODENUM];
int sub_id[SUBNODENUM];

void subnode_task(int id) {
  printf("\tSubnode %d reached successfully.\n", id);
}

void node_task(int id) {
  printf("Node %d reached successfully.\n", id);
#pragma omp task depend(out : sub_id[0])
  { subnode_task(0); }

  for (int i = 1; i < SUBNODENUM; i++) {
#pragma omp task depend(in : sub_id[i - 1]) depend(out : sub_id[i])
    { subnode_task(i); }
  }
#pragma omp taskwait
}

int main() {

#pragma omp parallel
  {
// start ring
#pragma omp single
    {
#pragma omp task depend(out : id[0])
      { node_task(0); }

      for (int i = 1; i < NODENUM; i++) {
#pragma omp task depend(in : id[i - 1]) depend(out : id[i])
        { node_task(i); }
      }
    }
  }

  return 0;
}
