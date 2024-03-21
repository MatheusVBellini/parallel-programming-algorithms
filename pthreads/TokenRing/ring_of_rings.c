#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_NODES 4
#define NUM_SUBNODES 4

int token;
sem_t node_sem[NUM_NODES];
sem_t subnode_sem[NUM_SUBNODES];

void* root_node(void) {
  
  printf("Ring Starting...\n");
  token = 0;

  sem_post(&node_sem[1 % NUM_NODES]); // free next node
  sem_wait(&node_sem[0]);             // wait its time

  printf("Ring finished with token value: %d\n", token);

  fflush(0);
  pthread_exit(0);

} 

void* common_subnode(void* ids) {
  
  int id = *((int*)ids);
  if (!id) pthread_exit(0);

  sem_wait(&subnode_sem[id]);
  token++;
  printf("\t\tSubnode %d called\n", id);
  sem_post(&subnode_sem[(id + 1) % NUM_SUBNODES]);

  fflush(0);
  pthread_exit(0);

}

void* common_node(void* ids) {

  // sub-node creation
  int sub_ids[NUM_SUBNODES];
  pthread_t subnode[NUM_SUBNODES];

  int id = *((int*)ids);
  
  sem_wait(&node_sem[id]);                     // wait current node
  token++;
  printf("Node %d activated\n", id);
  
  // sub-node instantiation
  for (int i = 0; i < NUM_SUBNODES; i++) {
    sem_init(&subnode_sem[i], 0, 0);          // sub-node semaphores begin all locked
    sub_ids[i] = i;                           // id for identifying sub-node
  }

  for (int i = 0; i < NUM_SUBNODES; i++) {
    if (pthread_create(&subnode[i], 0, (void *) common_subnode, (void *) &sub_ids[i]) != 0) { 
		  printf("Error creating thread! Exiting! \n");
		  exit(0);
	  }
  }

  // subnode routine
  sem_post(&subnode_sem[1 % NUM_SUBNODES]);
  
  // wait sub-nodes
  for(int i = 0; i < NUM_SUBNODES; i++)
    pthread_join(subnode[i], 0);
  fflush(0);

  sem_wait(&subnode_sem[0]);  // quit sub-node execution

  sem_post(&node_sem[(id + 1) % NUM_NODES]);  // free next node

  fflush(0);
  pthread_exit(0);

}

int main() {

  int ids[NUM_NODES];
  pthread_t node[NUM_NODES];

  for (int i = 0; i < NUM_NODES; i++) {
    sem_init(&node_sem[i], 0, 0);             // node semaphores begin all locked
    ids[i] = i;                               // id for identifying node
  }

  // initialize threads
  if (pthread_create(&node[0], 0, (void *) root_node, (void *) 0) != 0) { 
		printf("Error creating thread! Exiting! \n");
		exit(0);
	}
  for (int i = 1; i < NUM_NODES; i++) {
    if (pthread_create(&node[i], 0, (void *) common_node, (void *) &ids[i]) != 0) { 
		  printf("Error creating thread! Exiting! \n");
		  exit(0);
	  }
  }
  
  
  // sync threads
  for(int i = 0; i < NUM_NODES; i++)
    pthread_join(node[i], 0);
  fflush(0);

  return 0;

}
