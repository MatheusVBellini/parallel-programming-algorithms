#include <omp.h>
#include <stdio.h>

int main() {
#pragma omp parallel sections
  {
#pragma omp section
    {
      printf("Outer Section 1, executed by thread %d\n", omp_get_thread_num());
#pragma omp parallel sections
      {
#pragma omp section
        {
          printf("  Inner Section 1, executed by thread %d\n",
                 omp_get_thread_num());
        }

#pragma omp section
        {
          printf("  Inner Section 2, executed by thread %d\n",
                 omp_get_thread_num());
        }
      }
    }

#pragma omp section
    {
      printf("Outer Section 2, executed by thread %d\n", omp_get_thread_num());
    }
  }
  return 0;
}
