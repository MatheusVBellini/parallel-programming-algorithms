#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

int reduction_sum(int *list, int size) {
  int sum = 0;
  #pragma omp parallel for reduction(+ : sum)
  for (int i = 0; i < size; i++)
    sum += list[i];
  return sum;
} 

int *treefy(int first, int last) {
  int j = first, size = 1+last-first;
  int *list = (int *)malloc(size*sizeof(int));
  
  for (int i = 0; i < size; i++)
    list[i] = first++;

  return list;
}

int main() {
  int *tree, sum;
  int lb = 1, ub = 9;
  int size = 1+ub-lb;
  tree = treefy(lb,ub);
  sum = reduction_sum(tree,size);
  printf("%d\n", sum);

  free(tree);
  return 0;
}
