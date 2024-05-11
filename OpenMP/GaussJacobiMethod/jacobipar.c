#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
data_t random_number(void) {
  return (data_t)((rand() / (0.5 * RAND_MAX) - 1.0) * RAND_LIMIT);
}

/**
 * Allocate aligned memory
 *
 * @param size Size in bytes of how much space must be allocated
 * @return pointer to allocated data
 */
void *aligned_malloc(size_t size) {
  void* ptr = NULL;
  int ret = posix_memalign(&ptr, 32, size);
  if (ret) {
    printf("Memory allocation failed.\n");
    exit(1);
  }

  return ptr;
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
    #pragma omp parallel for num_threads(T) shared(linsys)
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
  data_t **m = (data_t **)aligned_malloc(N * sizeof(data_t *));

  #pragma omp parallel for num_threads(T) shared(m)
  for (int i = 0; i < N; i++) {
    m[i] = (data_t *)aligned_malloc(N * sizeof(data_t));
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
    #pragma omp for simd linear(i : 1) aligned(coeficients, matrix: 32)
    for (i = 0; i < N; i++) {
      coeficients[i] = -fabs(matrix[i][i]/(data_t)matrix[i][i]);
    }

    #pragma omp for collapse(2) reduction(+: coeficients[:N])
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        coeficients[i] += fabs(matrix[i][j]/(data_t)matrix[i][i]);
      }
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
  linsys->b = (data_t *)aligned_malloc(sizeof(data_t) * N);

  // value generation
  #pragma omp parallel for num_threads(T) shared(linsys)
  for (int i = 0; i < N; i++) {
    random_number();
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

    #pragma omp for simd collapse(2) aligned(normsys, linsys: 32)
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        normsys->A[i][j] = -linsys->A[i][j] / linsys->A[i][i];
      }
    }

    #pragma omp for simd linear(i : 1) aligned(normsys, linsys: 32)
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
    data_t *xm1 = (data_t *)aligned_malloc(sizeof(data_t) * N); // x minus 1 (x[k-1])
    data_t *xp1 = (data_t *)aligned_malloc(sizeof(data_t) * N); // m plus 1  (x[k+1])
    bool converged = 0;
    omp_lock_t move_data;
    omp_init_lock(&move_data);

    #pragma omp parallel num_threads(2) shared(xm1, x, xp1, e, converged)
    { 
      // first iteration
      #pragma omp for simd aligned(xm1, x, xp1: 32)
      for (int i = 0; i < N; i++){
        xm1[i] = x[i];                        // save x[k] in x[k-1]
        xp1[i] = normsys->b[i];
      }
      #pragma omp for simd collapse(2) reduction(+: xp1[:N]) aligned(xp1: 32)
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
          xp1[i] += normsys->A[i][j] * x[j];  // calculate x[k+1]
        }
      }
      #pragma omp for simd aligned(xm1, x, xp1: 32)
      for (int i = 0; i < N; i++){
        x[i] = xp1[i];                        // save x[k+1] in x[k]
      }
      
      #pragma omp single
      {
        // check convergence
        #pragma omp task
        {
          data_t ret;
          do {
            omp_set_lock(&move_data);
            ret = calc_err(xm1, x);
            if (ret <= e) {
              #pragma omp atomic write
              converged = 1;
              #pragma omp flush(converged)
            }
            omp_unset_lock(&move_data);
          } while(!converged);
        }
        
        // main loop
        #pragma omp task
        {
          #pragma omp parallel num_threads(T-1) shared(xm1, x, xp1, converged)         
          {
            while (!converged) {
              #pragma omp for simd aligned(xp1: 32)
              for (int i = 0; i < N; i++){
                xp1[i] = normsys->b[i];
              }
              #pragma omp for simd collapse(2) aligned(xp1, x: 32) reduction(+: xp1[:N])
              for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                  xp1[i] += normsys->A[i][j] * x[j];  // calculate x[k+1]
                }
              }
              
              // copy data to use in the other task
              #pragma omp single
              { 
                omp_set_lock(&move_data);
              }
              #pragma omp barrier
              #pragma omp for simd aligned(xm1, x: 32)
              for (int i = 0; i < N; i++)
                xm1[i] = x[i];               // x[k] in x[k-1]
              #pragma omp for simd aligned(xm1, x: 32)
              for (int i = 0; i < N; i++)
                x[i] = xp1[i];               // x[k+1] in x[k]
              #pragma omp single
              {
                omp_unset_lock(&move_data);
              }
              #pragma omp barrier
            }
          }
        }
      }
    }
    
    free(xm1);
    return xp1;
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
      printf("\tInvalid input. Equations are only indexed between 0 and %d. Try again!\n", N-1);
    }
    else {
      valid = 1;
    }
  } while (!valid);

  #pragma omp parallel for simd reduction(+: final_value) aligned(solution, linsys: 32) shared(choice, linsys, solution)
  for (int i = 0; i < N; i++) {
    final_value += solution[i]*linsys->A[choice][i];
  }

  printf("\n");
  printf("\tReal result:           %8.4lf\n", linsys->b[choice]);
  printf("\tResult using solution: %8.4lf\n", final_value);

}

int main(int argc, char *argv[]) {
  omp_set_max_active_levels(2); // enable 2 level nesting

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

  // generate linear system
  LinSys linsys;
  gen_linear_system(&linsys);
  linsys_print(&linsys);
  
  // normalize system
  LinSys normsys;
  normsys.A = allocate_matrix();
  normsys.b = (data_t *)aligned_malloc(sizeof(data_t) * N);
  normalize_system(&linsys, &normsys);

  // solve system
  data_t *x = (data_t *)aligned_malloc(N * sizeof(data_t));
  data_t *tmp = normsys.b;
  #pragma omp parallel for simd num_threads(T) aligned(x, tmp: 32)
  for (int i = 0; i < N; i++) {
    x[i] = tmp[i];
  }

  data_t *res = solve(&normsys, x, 0.00001);

  printf("\t[Solution]\n");
  for (int i = 0; i < N; i++)
    printf("\tx%d = %8.4lf\n", i, res[i]);

  // test solution validity
  test_solution(&linsys, res);

  // free allocated memory
  free(x);
  free(res);
  linsys_free(&linsys);
  linsys_free(&normsys);

  return 0;
}
