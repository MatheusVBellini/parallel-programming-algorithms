#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLI_ARG_NUM 2
#define RAND_LIMIT 1000

int N = 0;
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

// global flags and default values
bool OMIT_PRINT = false;
bool OMIT_TEST = false;

/**
 * Generate a random data_t number between -RAND_LIMIT and RAND_LIMIT
 *
 * @return random number
 */
data_t random_number(void) {
  return (data_t)((rand() / (0.5 * RAND_MAX) - 1.0) * RAND_LIMIT);
}

/**
 * Show linear system on screen
 *
 * @param linsys Linear system to be showed
 */
void linsys_print(LinSys *linsys) {
  data_t tmp = 0;
  printf("\t[Linear System]\n");
  for (int i = 0; i < N; i++) {
    printf("[%d]\t", i);
    for (int j = 0; j < N; j++) {
      tmp = linsys->A[i][j];
      if (j == 0) {
        printf("%9.4lf*x%d", tmp, j);
      } else if (tmp < 0)
        printf(" - %8.4lf*x%d", fabs(tmp), j);
      else
        printf(" + %8.4lf*x%d", tmp, j);
    }
    tmp = linsys->b[i];
    if (tmp < 0)
      printf(" = %8.4lf\n", linsys->b[i]);
    else
      printf(" =  %8.4lf\n", linsys->b[i]);
  }
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
data_t **allocate_matrix(void) {
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

  for (int i = 0; i < N; i++) {
    coeficients[i] = 0;
    for (int j = 0; j < N; j++) {
      if (i == j)
        continue;
      coeficients[i] += fabs(matrix[i][j]);
    }
    coeficients[i] /= (data_t)fabs(matrix[i][i]);

    if (coeficients[i] > max)
      max = coeficients[i];
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
    random_number();
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
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      normsys->A[i][j] = -linsys->A[i][j] / linsys->A[i][i];
    }
    normsys->A[i][i] = 0;
    normsys->b[i] = linsys->b[i] / linsys->A[i][i];
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
  for (int i = 0; i < N; i++) {
    res[i] = x[i];
  }

  do {
    for (int i = 0; i < N; i++) {
      x[i] = res[i];
    }
    for (int i = 0; i < N; i++) {
      res[i] = normsys->b[i];
    }
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        res[i] += normsys->A[i][j] * x[j];
      }
    }
  } while (calc_err(x, res) > e);

  return res;
}

/**
 * Ask the user to test the solution in some chosen equation
 *
 * @param linsys Original linear system
 * @param solution Vector of solution values
 */
void test_solution(LinSys *linsys, data_t *solution) {
  int choice = 0;
  bool valid = 0;
  data_t final_value = 0;

  printf("\n\t[TEST]\n");

  do {
    printf("\tIn which equation do you want to test the solution? ");
    scanf("%d", &choice);
    if (choice >= N || choice < 0) {
      printf("\tInvalid input. Equations are only indexed between 0 and %d. "
             "Try again!\n",
             N - 1);
    } else {
      valid = 1;
    }
  } while (!valid);

  for (int i = 0; i < N; i++) {
    final_value += solution[i] * linsys->A[choice][i];
  }

  printf("\n");
  printf("\tReal result:           %8.4lf\n", linsys->b[choice]);
  printf("\tResult using solution: %8.4lf\n", final_value);
}

void check_flags(char *argv[], int argc) {
  if (argc - 1 == CLI_ARG_NUM)
    return; // no flags
  int flag_start = CLI_ARG_NUM + 1;

  for (int i = flag_start; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0) {
      OMIT_PRINT = true;
    } else if (strcmp(argv[i], "-t") == 0) {
      OMIT_TEST = true;
    }
  }
}

int main(int argc, char *argv[]) {
  // argument check
  if ((argc - 1) < CLI_ARG_NUM) {
    printf("\nUsage: jacobiseq <N> <seed> <flags>\n\n");
    printf("N -> Matrix Order\n");
    printf("seed -> Seed for random number generations\n");
    printf("flags -> Additional flags for program configuration\n");
    exit(1);
  }

  // using arguments
  N = atoi(argv[1]);
  srand(atoi(argv[3]));

  check_flags(argv, argc);

  // generate linear system
  LinSys linsys;
  gen_linear_system(&linsys);

  if (!OMIT_PRINT)
    linsys_print(&linsys);

  // normalize system
  LinSys normsys;
  normsys.A = allocate_matrix();
  normsys.b = (data_t *)malloc(sizeof(data_t) * N);
  normalize_system(&linsys, &normsys);

  // solve system
  data_t *x = (data_t *)malloc(N * sizeof(data_t));
  memcpy(x, normsys.b, N * sizeof(data_t)); // initial bet for x

  data_t *res = solve(&normsys, x, 0.00001);

  if (!OMIT_PRINT) {
    printf("\t[Solution]\n");
    for (int i = 0; i < N; i++)
      printf("\tx%d = %8.4lf\n", i, res[i]);
  }

  // test solution validity
  if (!OMIT_TEST)
    test_solution(&linsys, res);

  // free allocated memory
  free(x);
  free(res);
  linsys_free(&linsys);
  linsys_free(&normsys);

  return 0;
}
