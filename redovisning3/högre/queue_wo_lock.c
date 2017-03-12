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
#include <limits.h>
#include <time.h>

typedef struct node {
  int val;
  struct node *next;
  struct node *prev;
} node;

node* front = NULL;
node* rear = NULL;

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
  newNode->next = front;

  // Base cases
  // Queue is empty
  if (front != NULL)
    front->prev = newNode;

  front = newNode;

  // Do everything before updating rear
  if (rear == NULL)
    rear = front;
}

int dequeue() {
  long stuck = 0;
  while (rear == NULL) {
    stuck++;
    if (stuck > 2000000000) {
      printf("I am stuck!!!!\n");
      printf("front is at: %p\n", front);
      printf("rear is at: %p\n", rear);
      exit(0);
    }
  }

  node* dequeuedNode = rear;
  // We are about to remove the last node
  if (rear == front) {
    if (rear == front) {
      front = NULL;
      rear = NULL;
    } else {

      return dequeue();
    }
  } else {
    // Normal case
    rear = rear->prev;
    if (rear != NULL)
      rear->next = NULL;
  }

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

  args *arg = malloc((n + 1) * sizeof(arg));

  struct timespec t_start, t_stop;
  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);
  // Enqueue benchmark
  arg[0].num = inc;
  benchEnqueue(&arg[0]);

  // Now for the dequeue
  benchDequeue(&arg[0]);

  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

  long wall_sec = t_stop.tv_sec - t_start.tv_sec;
  long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec;
  long wall_msec = (wall_sec * 1000) + (wall_nsec / 1000000);

  printf("done in %ld ms\n", wall_msec);
  print_queue();

  return 0;
}
