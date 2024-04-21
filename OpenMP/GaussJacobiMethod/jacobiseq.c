#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CLI_ARG_NUM 3

int N = 0;

/**
 * Test if matrix converges in the Jacobi-Gauss method
 *
 * @param matrix float**
 */
char convergence_test(float **matrix) {
  /* lines criterium
   *
   * a_k = ( \sum_{j=1 && j!=k}^{n} |a_{ik}| ) / |a_{kk}|
   * max(a_k) < 1
   *
   */

  float max = -1;
  float coeficients[N];

  for (int i = 0; i < N; i++) {

    coeficients[i] = 0;
    for (int j = 0; j < N; j++) {
      if (i == j)
        continue;
      coeficients[i] += fabs(matrix[i][j]);
    }
    coeficients[i] /= fabs(matrix[i][i]);

    if (coeficients[i] > max)
      max = coeficients[i];
  }

  return max < 1;
}

int main(int argc, char *argv[]) {

  // dev info
  printf("No de Argumentos: %d\n", argc);
  for (int i = 0; i < argc; i++)
    printf("Argumentos: %s\n", argv[i]);

  // argument check
  if ((argc - 1) != CLI_ARG_NUM) {
    printf("\nUsage: jacobiseq <N> <T> <seed>\n\n");
    printf("N -> Matrix Order\n");
    printf("T -> Total number of threads in the application\n");
    printf("seed -> Seed for random number generations\n");
    exit(1);
  }

  // using arguments
  N = atoi(argv[1]);

  // DEBUG: convergence_test
  float **m = (float **)malloc(N * sizeof(float *));
  if (!m) {
    printf("Failed to allocate memory for the matrix");
  }
  for (int i = 0; i < N; i++) {
    m[i] = (float *)malloc(N * sizeof(float));
    if (!m[i]) {
      printf("Failed to allocate memory for the matrix");
    }
  }
  char ret = convergence_test(m);
  printf("%d\n", ret);

  return 0;
}
