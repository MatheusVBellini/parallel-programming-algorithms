#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRODUCED 10
#define QUEUE_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

int queue[QUEUE_SIZE], produced = 0, consumed = 0, current = 0;
omp_lock_t queue_lock;

void consume() {
  int product = queue[--current];
  printf("\t\tConsumer removed: %d\tConsumption No: %d\n", product, consumed);
}

void consumer() {
  while (1) {
    // check if queue has any products in it
    omp_set_lock(&queue_lock);
    if (consumed >= MAX_PRODUCED) {
      omp_unset_lock(&queue_lock);
      break;
    }
    if (current <= 0) {
      omp_unset_lock(&queue_lock);
      continue;
    }
    consumed++;
    // remove a product from the queue
    consume();
    omp_unset_lock(&queue_lock);
  }
}

void insert(int product) {
  queue[current++] = product;
  printf("Producer inserted: %d\tProduction No: %d\n", product, produced);
}

void producer() {
  int product;
  while (1) {
    product = rand() % 20;
    // check if queue has space for new product
    omp_set_lock(&queue_lock);
    if (produced >= MAX_PRODUCED) {
      omp_unset_lock(&queue_lock);
      break;
    }
    if (current >= QUEUE_SIZE) {
      omp_unset_lock(&queue_lock);
      continue;
    }
    produced++;
    // insert product into queue
    insert(product);
    omp_unset_lock(&queue_lock);
  }
}

int main() {
  omp_init_lock(&queue_lock);

#pragma omp parallel
  {
    for (int i = 0; i < NUM_PRODUCERS; i++) {
#pragma omp task
      { producer(); }
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
#pragma omp task
      { consumer(); }
    }
  }

  omp_destroy_lock(&queue_lock);
  return 0;
}
