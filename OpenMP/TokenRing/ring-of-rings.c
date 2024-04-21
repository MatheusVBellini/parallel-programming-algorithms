#include <omp.h>
#include <stdio.h>

#define NODENUM 4
#define SUBNODENUM 4

int sub_id[SUBNODENUM];

void subnode_task(int id) {
  printf("\tSubnode %d reached successfully.\n", id);
}

void node_task(int id) {
  printf("Node %d reached successfully.\n", id);
#pragma omp task depend(out : sub_id[0])
  { subnode_task(sub_id[0]); }

  for (int i = 1; i < SUBNODENUM; i++) {
#pragma omp task depend(in : sub_id[i - 1]) depend(out : sub_id[i])
    { subnode_task(sub_id[i]); }
  }
#pragma omp taskwait
}

int main() {
  int id[NODENUM];

#pragma omp parallel
  {
// fill up id vector
#pragma omp for nowait
    for (int i = 0; i < NODENUM; i++) {
      id[i] = i;
    }

// fill up sub_id vector
#pragma omp for
    for (int i = 0; i < SUBNODENUM; i++) {
      sub_id[i] = i;
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
