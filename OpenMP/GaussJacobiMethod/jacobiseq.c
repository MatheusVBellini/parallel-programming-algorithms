#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CLI_ARG_NUM 3
#define RAND_LIMIT 1000

int N = 0;

/**
 * Test if matrix converges in the Jacobi-Gauss method
 *
 * @param matrix float** - matrix to be checked
 * @return 0 if doesn't converge, not 0 if converges
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

/**
 * Generate a matrix that satisfies the convergence test
 *
 * @return matrix filled with randomly generated values that pass the
 * convergence test
 */
float **gen_matrix() {
  // memory allocation
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

  // value generation
  do {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) { // random fill up
        m[i][j] = RAND_LIMIT * (rand() / (float)RAND_MAX);
      }
    }
  } while (!convergence_test(m));

  return m;
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
  srand(atoi(argv[3]));

  // DEBUG: gen_matrix
  float **m = gen_matrix();
  char ret = convergence_test(m);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%f ", m[i][j]);
    }
    printf("\n");
  }
  printf("\n%d\n", ret);

  return 0;
}
