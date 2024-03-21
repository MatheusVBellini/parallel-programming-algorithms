#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <semaphore.h>

#define QUEUE_SIZE 10
#define MAX_PRODUCED 15

#define NUM_PROD 5
#define NUM_CONS 5

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
  int flag = 1;

  while (flag) {
    item = (int) rand() % 10;

    sem_wait(&empty);
    sem_wait(&mutex);
    
    // critical region
    if (produced < MAX_PRODUCED)
      insert(item);
    else
      flag = 0;

    sem_post(&mutex);
    sem_post(&full);
  }

  fflush(0);
  pthread_exit(0);

}

void* consumer(void) {
  int flag = 1;

  while (flag) {
    sem_wait(&full);
    sem_wait(&mutex);
    
    // critical region
    if (consumed < MAX_PRODUCED)
      consume();
    else
      flag = 0;

    sem_post(&mutex);
    sem_post(&empty);
  }

  fflush(0);
  pthread_exit(0);

}

int main() {

  pthread_t consumer_thread[NUM_CONS], producer_thread[NUM_PROD];
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, QUEUE_SIZE);
  sem_init(&mutex, 0, 1);
  
  for (int i = 0; i < NUM_PROD; i++) {
    if (pthread_create(&producer_thread[i], 0, (void *) producer, (void *) 0) != 0) {
		  printf("Error creating thread producer! Exiting! \n");
		  exit(0);
	  }
  }
  for (int i = 0; i < NUM_CONS; i++) {
    if (pthread_create(&consumer_thread[i], 0, (void *) consumer, (void *) 0) != 0) {
		  printf("Error creating thread consumer! Exiting! \n");
		  exit(0);
	  }
  }
  
  for (int i = 0; i < NUM_PROD; i++) 
    pthread_join(producer_thread[i], 0);
  for (int i = 0; i < NUM_CONS; i++)
    pthread_join(consumer_thread[i], 0);
  fflush(0);

  return 0;

}
