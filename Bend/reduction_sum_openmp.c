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
  int size;

  for (int i = 2; i <= 2000; i++) {
    size = i;
    tree = treefy(1,i);
    sum = reduction_sum(tree,size);
    printf("%d ", sum);
    free(tree);
  }
  printf("\n");

  return 0;
}
