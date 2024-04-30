#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CLI_ARG_NUM 3
#define RAND_LIMIT 1000

int N = 0;
int T = 1;
typedef double data_t;

typedef struct {
  // Ax = b
  data_t **A;
  data_t *b;
} LinSys;

/**
 * Jacobi-Gauss Method converges if the matrix is diagonally dominant.
 *
 * A square matrix is said to be diagonally dominant matrix if for every row of
 * the matrix, the magnitude of the diagonal entry in a row is larger than or
 * equal to the sum of the magnitudes of all the other (non-diagonal) entries in
 * that row.
 */

/**
 * Generate a random data_t number between -RAND_LIMIT and RAND_LIMIT
 *
 * @return random number
 */
data_t random_number() {
  return (data_t)((rand() / (0.5 * RAND_MAX) - 1.0) * RAND_LIMIT);
}

/**
 * Show linear system on screen
 *
 * @param linsys Linear system to be showed
 */
void linsys_print(LinSys *linsys) {
  printf("\nA:\n");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%.5lf ", linsys->A[i][j]);
    }
    printf("\n");
  }
  printf("\nb:\n");
  for (int i = 0; i < N; i++)
    printf("%.5lf ", linsys->b[i]);
  printf("\n");
}

/**
 * Free allocated memory in a LinSys instance
 *
 * @param linsys Struct to be deallocated
 */
void linsys_free(LinSys *linsys) {
  if (linsys->A) {
    for (int i = 0; i < N; i++) {
      free(linsys->A[i]);
    }
    free(linsys->A);
  }
  if (linsys->b)
    free(linsys->b);
}

/**
 * Allocate matrix of order N
 *
 * @return pointer to allocated matrix
 */
data_t **allocate_matrix() {
  data_t **m = (data_t **)malloc(N * sizeof(data_t *));
  if (!m) {
    printf("Failed to allocate memory for the matrix");
  }
  for (int i = 0; i < N; i++) {
    m[i] = (data_t *)malloc(N * sizeof(data_t));
    if (!m[i]) {
      printf("Failed to allocate memory for the matrix");
    }
  }

  return m;
}

/**
 * Test if matrix converges in the Jacobi-Gauss method
 *
 * @param matrix Matrix to be checked
 * @return 0 if doesn't converge, not 0 if converges
 */
bool convergence_test(data_t **matrix) {
  /* lines criterium
   *
   * a_k = ( \sum_{j=1 && j!=k}^{n} |a_{ik}| ) / |a_{kk}|
   * max(a_k) < 1
   *
   */

  data_t max = -1;
  data_t coeficients[N];
  int i = 0, j = 0;

#pragma omp parallel private(i, j) shared(coeficients) num_threads(T)
  {
#pragma omp for simd linear(i : 1) linear(j : 1)
    for (i = 0; i < N; i++) {
      coeficients[i] = -fabs(matrix[i][i]);
      for (j = 0; j < N; j++) {
        coeficients[i] += fabs(matrix[i][j]);
      }
      coeficients[i] /= (data_t)fabs(matrix[i][i]);
    }

#pragma omp for reduction(max : max)
    for (i = 0; i < N; i++) {
      if (coeficients[i] > max)
        max = coeficients[i];
    }
  }

  return max < 1;
}

/**
 * Generate a linear system that satisfies the convergence test
 *
 * @param linsys Recipient of the system
 */
void gen_linear_system(LinSys *linsys) {
  linsys->A = allocate_matrix();
  linsys->b = (data_t *)malloc(sizeof(data_t) * N);

  // value generation
  for (int i = 0; i < N; i++) {
    // filling up b
    linsys->b[i] = random_number();

    // filling up A
    data_t row_sum = 0;
    for (int j = 0; j < N; j++) {
      linsys->A[i][j] = random_number();

      // row_sum doesn't include the diagonal element
      if (i == j)
        continue;
      row_sum += fabs(linsys->A[i][j]);
    }

    // turning A into a diagonally dominant matrix
    // F = row_sum / |a_ii|
    // for all j, a_ij = a_ij / F
    // hereby, |a_ii| > row_sum for all i
    data_t F = ((row_sum + fabs(random_number())) / fabs(linsys->A[i][i]));
    for (int j = 0; j < N; j++) {
      if (i == j)
        continue;
      linsys->A[i][j] /= F;
    }
  }

  if (!convergence_test(linsys->A)) {
    printf("Error generating convergent linear system!\n");
  }
}

/**
 * Normalize the system to apply the algorithm
 *
 * @param linsys Original linear system
 * @param normsys Recipient of the normalized system
 */
void normalize_system(LinSys *linsys, LinSys *normsys) {
  int i = 0, j = 0;

#pragma omp parallel num_threads(T) shared(linsys, normsys) private(i, j)
  {

#pragma omp for simd collapse(2)
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        normsys->A[i][j] = -linsys->A[i][j] / linsys->A[i][i];
      }
    }

#pragma omp for simd linear(i : 1)
    for (i = 0; i < N; i++) {
      normsys->A[i][i] = 0;
      normsys->b[i] = linsys->b[i] / linsys->A[i][i];
    }
  }
}

/**
 * Calculate error function
 *
 * @param x0 Original vector
 * @param x1 Post-iteration vector
 * @return error value
 */
data_t calc_err(data_t *x0, data_t *x1) {
  data_t max_diff = -1;
  data_t max_abs = -1;
  for (int i = 0; i < N; i++) {
    if (fabs(x1[i]) > max_abs)
      max_abs = fabs(x1[i]);
    if (fabs(x1[i] - x0[i]) > max_diff)
      max_diff = fabs(x1[i] - x0[i]);
  }

  return max_diff / max_abs;
}

/**
 * Solve the linear system
 *
 * @param normsys Normalized linear system
 * @param x Vector of initial values
 * @param e Result precision
 * @return result vector
 */
data_t *solve(LinSys *normsys, data_t *x, data_t e) {
  data_t *res = (data_t *)malloc(sizeof(data_t) * N);
  if (!res) {
    printf("Failed to allocate memory\n");
    exit(1);
  }

  // fill up for first interaction
  for (int i = 0; i < N; i++)
    res[i] = x[i];

  do {
    for (int i = 0; i < N; i++)
      x[i] = res[i];
    for (int i = 0; i < N; i++) {
      res[i] = normsys->b[i];
      for (int j = 0; j < N; j++) {
        res[i] += normsys->A[i][j] * x[j];
      }
    }
  } while (calc_err(x, res) > e);

  return res;
}

int main(int argc, char *argv[]) {

  // dev info
  printf("No de Argumentos: %d\n", argc);
  for (int i = 0; i < argc; i++)
    printf("Argumentos: %s\n", argv[i]);

  // argument check
  if ((argc - 1) != CLI_ARG_NUM) {
    printf("\nUsage: jacobipar <N> <T> <seed>\n\n");
    printf("N -> Matrix Order\n");
    printf("T -> Total number of threads in the application\n");
    printf("seed -> Seed for random number generations\n");
    exit(1);
  }

  // using arguments
  N = atoi(argv[1]);
  T = atoi(argv[2]);
  srand(atoi(argv[3]));

  // DEBUG: gen_linear_system
  LinSys linsys;
  gen_linear_system(&linsys);
  char ret = convergence_test(linsys.A);

  linsys_print(&linsys);
  printf("\n%d\n", ret);
  // end of gen_linear_system debug

  // DEBUG: normalize_system
  LinSys normsys;
  normsys.A = allocate_matrix();
  normsys.b = (data_t *)malloc(sizeof(data_t) * N);

  normalize_system(&linsys, &normsys);
  linsys_print(&normsys);
  // end of normalize_system debug

  // DEBUG: solve
  data_t *x = (data_t *)malloc(N * sizeof(data_t));
  for (int i = 0; i < N; i++)
    x[i] = (rand() / (data_t)RAND_MAX);

  printf("\nx0:\n");
  for (int i = 0; i < N; i++)
    printf("%f ", x[i]);
  printf("\n\n");

  data_t *res = solve(&normsys, x, 0.00001);

  printf("\n\nResult:\n");
  for (int i = 0; i < N; i++)
    printf("%f ", res[i]);
  printf("\n");
  // end of solve debug

  // free allocated memory
  free(x);
  free(res);
  linsys_free(&linsys);
  linsys_free(&normsys);

  return 0;
}
