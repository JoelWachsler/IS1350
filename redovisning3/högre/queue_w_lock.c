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
#include <limits.h>
#include <time.h>

typedef struct node {
  int val;
  struct node *next;
  struct node *prev;
} node;

node* front = NULL;
node* rear = NULL;
pthread_mutex_t mutexFront = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexRear = PTHREAD_MUTEX_INITIALIZER;

void print_queue() {
  node* current = front;

  while (current != NULL) {
    printf("%d -> ", current->val);
    current = current->next;
  }
  printf("\n");
}

int enqueue(int item) {
  node* newNode = malloc(sizeof(node));
  newNode->val = item;
  newNode->prev = NULL;
  pthread_mutex_lock(&mutexFront);
  newNode->next = front;

  // Base cases
  // Queue is empty
  if (front != NULL)
    front->prev = newNode;

  front = newNode;

  // Do everything before updating rear
  if (rear == NULL)
    rear = front;

  pthread_mutex_unlock(&mutexFront);
}

int dequeue() {
  pthread_mutex_lock(&mutexRear);
  long stuck = 0;
  while (rear == NULL);

  node* dequeuedNode = rear;
  // We are about to remove the last node
  if (rear == front) {
    pthread_mutex_lock(&mutexFront);
    if (rear == front) {
      front = NULL;
      rear = NULL;
    } else {
      pthread_mutex_unlock(&mutexFront);
      pthread_mutex_unlock(&mutexRear);

      return dequeue();
    }
    pthread_mutex_unlock(&mutexFront);
  } else {
    // Normal case
    rear = rear->prev;
    if (rear != NULL)
      rear->next = NULL;
  }

  pthread_mutex_unlock(&mutexRear);

  return dequeuedNode->val;
}

void* benchEnqueue(void* num) {
  for(int i = 0; i < *(int*)num; i++)
    enqueue(rand() % 100);
}

void* benchDequeue(void* num) {
  for(int i = 0; i < *(int*)num; i++)
    dequeue();
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
  pthread_t *threads2 = malloc(n * sizeof(pthread_t));
  args *arg = malloc((n + 1) * sizeof(arg));

  struct timespec t_start, t_stop;
  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);
  // Enqueue benchmark
  for(int i = 0; i < n; i++) {
    arg[i].num = inc;
    pthread_create(&threads[i], NULL, benchEnqueue, &arg[i]);
  }

  // Now for the dequeue
  for(int i = 0; i < n; i++)
    pthread_create(&threads2[i], NULL, benchDequeue, &arg[i]);

  for(int i = 0; i < n; i++)
    pthread_join(threads[i], NULL);
  printf("thread1 done\n");
  for(int i = 0; i < n; i++)
    pthread_join(threads2[i], NULL);
  printf("thread2 done\n");

  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

  long wall_sec = t_stop.tv_sec - t_start.tv_sec;
  long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec;
  long wall_msec = (wall_sec * 1000) + (wall_nsec / 1000000);

  printf("done in %ld ms\n", wall_msec);
  print_queue();

  free(threads);
  free(threads2);

  return 0;
}
