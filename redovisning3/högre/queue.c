/*
 * =====================================================================================
 *
 *       Filename:  queue.c
 *
 *    Description:  Queue implementation with thread safety
 *
 *        Version:  1.0
 *        Created:  2017-02-18 21:11:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joel Wachsler
 *   Organization:  None
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>

typedef struct node {
  int val;
  struct node *next;
  struct node *prev;
} node;

node* front = NULL;
node* rear = NULL;
pthread_mutex_t mutexFront = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexRear = PTHREAD_MUTEX_INITIALIZER;

int enqueue(int item) {
  node* newNode = malloc(sizeof(node));
  newNode->val = item;
  newNode->prev = NULL;
  pthread_mutex_lock(&mutexFront);
  newNode->next = front;

  // We've locked the front so nothing can happend until
  // we release it
  front = newNode;

  // Base cases
  // Queue is empty
  if (front->next != NULL)
    front->next->prev = newNode;
  else
    rear = front;

  pthread_mutex_unlock(&mutexFront);
}

int dequeue() {
  pthread_mutex_lock(&mutexRear);
  while (rear == NULL); // Wait until there is an item to dequeue
  node* dequeuedNode = rear;
  rear = rear->prev;

  // Base case for printing help
  if (rear != NULL)
    rear->next = NULL;
  else {
    pthread_mutex_lock(&mutexFront);
    if (rear == NULL)
      front = NULL;
    pthread_mutex_unlock(&mutexFront);
  }

  pthread_mutex_unlock(&mutexRear);

  int val = dequeuedNode->val;
  free(dequeuedNode);
  return val;
}

void print_queue() {
  node* current = front;

  while (current != NULL) {
    printf("%d -> ", current->val);
    current = current->next;
  }
  printf("\n");
}

void* benchEnqueue(void* num) {
  for(int i = 0; i < *(int*)num; i++)
    enqueue(rand() % INT_MAX);
}

void* benchDequeue(void* num) {
  for(int i = 0; i < *(int*)num; i++)
    dequeue(rand() % 100);
}

typedef struct {int num;} args;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: list <total> <threads>\n");
    exit(0);
  }
  int n = atoi(argv[2]);
  int inc = (atoi(argv[1]) / n);

  printf("%d threads doing %d operations each\n", n, inc);

  pthread_t *threads = malloc(n * sizeof(pthread_t));
  args *arg = malloc(n * sizeof(arg));

  // Enqueue benchmark
  for(int i = 0; i < n; i++) {
    arg[i].num = inc;
    pthread_create(&threads[i], NULL, benchEnqueue, &arg[i]);
  }

  for(int i = 0; i < n; i++)
    pthread_join(threads[i], NULL);

  // Now for the dequeue
  for(int i = 0; i < n; i++)
    pthread_create(&threads[i], NULL, benchDequeue, &arg[i]);

  for(int i = 0; i < n; i++)
    pthread_join(threads[i], NULL);

  printf("Final queue\n");
  print_queue();

  return 0;
}
