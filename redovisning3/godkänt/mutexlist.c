/*
 * =====================================================================================
 *
 *       Filename:  list.c
 *
 *    Description:  List with safe multithreaded access
 *
 *        Version:  1.0
 *        Created:  2017-02-17 15:40:31
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
#include <stdbool.h>
#include <time.h>

typedef struct node {
  int val;
  struct node *next;
  pthread_mutex_t mutex;
} node;

// The list can contain all ints
node sentinel = {INT_MAX, NULL, PTHREAD_MUTEX_INITIALIZER};
node dummy = {INT_MIN, &sentinel, PTHREAD_MUTEX_INITIALIZER};
node *list = &dummy;

/*
 * Searches the sorted list after the sought element
 * is found or we crash into the sentinel.
 *
 * The risk of another thread changing the current item
 * in this iteration is fairly low depending on the
 * length of the list
 */
bool contains(int item) {
  node *current = list->next;

  while (current->val < item)
    current = current->next;

  return current->val == item;
}

void insert(int item) {
  node *prev = list;
  pthread_mutex_lock(&prev->mutex);
  node *current = list->next;
  pthread_mutex_lock(&current->mutex);

  while (item > current->val) {
    pthread_mutex_unlock(&prev->mutex);
    prev = current;
    current = current->next;
    pthread_mutex_lock(&current->mutex);
  }

  node *new = malloc(sizeof(node));
  new->val = item;
  new->next = current;
  pthread_mutex_init(&new->mutex, NULL);
  prev->next = new;

  pthread_mutex_unlock(&prev->mutex);
  pthread_mutex_unlock(&current->mutex);
}

void delete(int item) {
  node *prev = list;
  pthread_mutex_lock(&prev->mutex);
  node *current = list->next;
  pthread_mutex_lock(&current->mutex);

  while (item > current->val) {
    pthread_mutex_unlock(&prev->mutex);
    prev = current;
    current = current->next;
    pthread_mutex_lock(&current->mutex);
  }

  node *removed = NULL;

  if (current->val == item) {
    prev->next = current->next;
    removed = current;
  }

  pthread_mutex_unlock(&prev->mutex);
  pthread_mutex_unlock(&current->mutex);

  if (removed != NULL) free(removed);
  return;
}

void print_list() {
  node *current = list->next;
  while (current->next != NULL) {
    printf("%d -> ", current->val);
    current = current->next;
  }
  printf("\n");
}

void *procedural_test() {
  int items[] = {100,150,4,90,11,564};
  for(int i = 0; i < sizeof(items)/sizeof(items[0]); i++) {
    printf("Inserting: %d\n", items[i]);
    insert(items[i]);
    print_list();
  }

  for(int i = 0; i < sizeof(items)/sizeof(items[0]); i++) {
    printf("Deleting: %d\n", items[i]);
    delete(items[i]);
    print_list();
  }

  printf("Thread done, printing:\n");
  print_list();
}

void concurrent_test() {
  pthread_t one;
  pthread_t two;
  pthread_create(&one, NULL, procedural_test, NULL);
  pthread_create(&two, NULL, procedural_test, NULL);
  pthread_join(one, NULL);
  pthread_join(two, NULL);
}

typedef struct {int start; int stop;} args;

void *benchInsert(void* arg) {
  int start = ((args*)arg)->start;
  int stop = ((args*)arg)->stop;
  for(int i = start; i < stop; i++)
    insert(i);
}

void *benchDelete(void* arg) {
  int start = ((args*)arg)->start;
  int stop = ((args*)arg)->stop;
  for(int i = start; i < stop; i++)
    delete(i);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: list <total> <threads>\n");
    exit(0);
  }
  int n = atoi(argv[2]);
  int inc = (atoi(argv[1]) / n);

  printf("%d threads doing %d operations each\n", n, inc);

  // Allocate threads
  pthread_t *threads = malloc(n * sizeof(pthread_t));
  // Allocate args
  args *arg = malloc(n * sizeof(args));
  for(int i = 0; i < n; i++) {
    arg[i].start = i * inc;
    arg[i].stop = (i + 1) * inc;
  }

  struct timespec t_start, t_stop;
  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);
  // Start the threads
  for(int i = 0; i < n; i++)
    pthread_create(&threads[i], NULL, benchInsert, &arg[i]);

  // Wait for the threads to finish
  for(int i = 0; i < n; i++)
    pthread_join(threads[i], NULL);

  for(int i = 0; i < n; i++)
    pthread_create(&threads[i], NULL, benchDelete, &arg[i]);
  for(int i = 0; i < n; i++)
    pthread_join(threads[i], NULL);

  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

  long wall_sec = t_stop.tv_sec - t_start.tv_sec;
  long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec;
  long wall_msec = (wall_sec * 1000) + (wall_nsec / 1000000);

  printf("done in %ld ms\n", wall_msec);
  printf("Program finished, printing list:\n");
  print_list();

  return 0;
}
