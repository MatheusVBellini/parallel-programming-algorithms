#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <semaphore.h>

#define QUEUE_SIZE 10
#define MAX_PRODUCED 15

sem_t empty, full, mutex;
int queue[QUEUE_SIZE];
int queue_top = 0, produced = 0, consumed = 0;

void insert(int item) {
  produced++;
  queue[queue_top++] = item;
  printf("Produced item #%d: %d\n", produced, item);
}

void consume(void) {
  consumed++;
  int item = queue[--queue_top];
  printf("Consumed item #%d: %d\n", consumed, item);
}

void* producer(void) {
  int item; 

  while (produced < MAX_PRODUCED) {
    item = (int) rand() % 10;

    sem_wait(&empty);
    sem_wait(&mutex);
    
    // critical region
    insert(item);

    sem_post(&mutex);
    sem_post(&full);
  }

  fflush(0);
  pthread_exit(0);

}

void* consumer(void) {

  while (consumed < MAX_PRODUCED) {
    sem_wait(&full);
    sem_wait(&mutex);
    
    // critical region
    consume();

    sem_post(&mutex);
    sem_post(&empty);
  }

  fflush(0);
  pthread_exit(0);

}

int main() {

  pthread_t consumer_thread, producer_thread;
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, QUEUE_SIZE);
  sem_init(&mutex, 0, 1);

  if (pthread_create(&producer_thread, 0, (void *) producer, (void *) 0) != 0) {
		printf("Error creating thread producer! Exiting! \n");
		exit(0);
	}
  if (pthread_create(&consumer_thread, 0, (void *) consumer, (void *) 0) != 0) {
		printf("Error creating thread consumer! Exiting! \n");
		exit(0);
	}

  pthread_join(producer_thread, 0);
  pthread_join(consumer_thread, 0);
  fflush(0);

  return 0;

}
