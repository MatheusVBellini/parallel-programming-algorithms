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
 * Show linear system on screen
 *
 * @param linsys LinSys* - linear system to be showed
 */
void print_linsys(LinSys* linsys) {
  printf("\nA:\n");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%f ", linsys->A[i][j]);
    }
    printf("\n");
  }
  printf("\nb:\n");
  for (int i = 0; i < N; i++) printf("%f ", linsys->b[i]);
  printf("\n");
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
 * Generate a linear system that satisfies the convergence test
 *
 * @param linsys LinSys* - recipient of the system
 */
void gen_linear_system(LinSys *linsys) {

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

/**
 * Normalize the system to apply the algorithm
 *
 * @param linsys LinSys* - original linear system
 * @param normsys LinSys* - recipient of the normalized system
 */
void normalize_system(LinSys* linsys, LinSys* normsys) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      normsys->A[i][j] = -linsys->A[i][j]/linsys->A[i][i];
    }
    normsys->A[i][i] = 0;
    normsys->b[i] = linsys->b[i]/linsys->A[i][i];
  }
}

/**
 * Calculate error function
 *
 * @param x0 float* - original vector
 * @param x1 float* - post-iteration vector
 * @return error value
 */
float calc_err(float* x0, float* x1) {
  float max_diff = -1;
  float max_abs = -1;
  for (int i = 0; i < N; i++) {
    if (fabs(x1[i]) > max_abs) max_abs = fabs(x1[i]);
    if (fabs(x1[i] - x0[i]) > max_diff) max_diff = fabs(x1[i] - x0[i]);
  }

  return max_diff / max_abs;
}

/**
 * Solve the linear system
 *
 * @param normsys LinSys* - normalized linear system
 * @param e float - result precision
 * @return result vector
 */
float* solve(LinSys* normsys, float e) {
  float *res = (float*)malloc(sizeof(float)*N);
  
  //do {

  //} while (calc_err(res,normsys->b) > e);

  return res;
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
  linsys.A = allocate_matrix();
  linsys.b = (float *)malloc(sizeof(float)*N);

  gen_linear_system(&linsys);
  char ret = convergence_test(linsys.A);

  print_linsys(&linsys);
  printf("\n%d\n", ret);
  // end of gen_linear_system debug


  // DEBUG: normalize_system
  LinSys normsys;
  normsys.A = allocate_matrix();
  normsys.b = (float *)malloc(sizeof(float)*N);

  normalize_system(&linsys,&normsys);
  print_linsys(&normsys);
  // end of normalize_system debug

  return 0;
}
