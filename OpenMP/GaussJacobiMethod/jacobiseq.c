#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CLI_ARG_NUM 3
#define RAND_LIMIT 1000

int N = 0;
typedef struct {
  // Ax = b
  float **A;
  float *b;
} LinSys;

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
    coeficients[i] /= (float)fabs(matrix[i][i]);

    if (coeficients[i] > max)
      max = coeficients[i];
  }

  return max < 1;
}

/**
 * Allocate matrix of order N
 *
 * @return pointer to allocated matrix
 */
float **allocate_matrix() {
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

  return m;
}

/**
 * Generate a linear system that satisfies the convergence test
 *
 * @return convergent linear system
 */
void gen_linear_system(LinSys *linsys) {
  linsys->A = allocate_matrix();
  linsys->b = (float *)malloc(sizeof(float)*N);

  // value generation
  do {
    for (int i = 0; i < N; i++) {
      linsys->b[i] = RAND_LIMIT * (rand() / (float)RAND_MAX);
      for (int j = 0; j < N; j++) { // random fill up
        linsys->A[i][j] = RAND_LIMIT * (rand() / (float)RAND_MAX);
      }
    }
  } while (!convergence_test(linsys->A));
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

  // DEBUG: gen_linear_system
  LinSys linsys;
  gen_linear_system(&linsys);
  char ret = convergence_test(linsys.A);

  printf("\nA:\n");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%f ", linsys.A[i][j]);
    }
    printf("\n");
  }
  printf("b:\n");
  for (int i = 0; i < N; i++) printf("%f ", linsys.b[i]);
  printf("\n\n%d\n", ret);
  // end of gen_linear_system debug

  return 0;
}
