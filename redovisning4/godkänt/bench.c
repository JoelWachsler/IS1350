/*
 * =====================================================================================
 *
 *       Filename:  bench.c
 *
 *    Description:  Benchmarks the read speed of the file provided
 *
 *        Version:  1.0
 *        Created:  2017-02-27 12:03:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Joel Wachsler
 *   Organization: KTH
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

// Number of blocks to create
#define BLOCKS 1024*1024
// Size of each block
#define SIZE 512
// A number of read operations seperated by STEP bytes
#define STEP 64
// Read from COUNT number of blocks
#define COUNT 1000

// Used for sorting the result
int compare(const void *a, const void *b) {
  return (int)(*(long *)a - *(long *)b);
}

/*
 * We want the execution time in nano-seconds.
 * Therefore we provide a function which takes two timespec entries
 * and returns the difference as long.
 */
long n_sec(struct timespec *start, struct timespec *stop) {
  long diff_sec   = stop->tv_sec  - start->tv_sec;
  long diff_nsec  = stop->tv_nsec - start->tv_nsec;
  long wall_nsec  = (diff_sec * 1000000000) + diff_nsec;
  return wall_nsec;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: read <file name>\n");
    return -1;
  }
  char *name = argv[1];

  // Open the file to be benchmarked.
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int flag = O_RDWR | O_CREAT;
  /* int flag = O_RDWR | O_CREAT | O_DSYNC; */
  int fd = open(name, flag, mode);
  assert(fd != -1);

  // Number of places in the file to benchmark
  int entries  = SIZE / STEP;

  // The table holds an array of timestamps for each of the indexes
  // we access i.e 0, 64, 128, ... Each of these arrays are COUNT long.
  long **table = malloc(sizeof(long*) * entries);

  for(int i = 0; i < entries; i++)
    table[i] = malloc(sizeof(long) * COUNT);

  // The actual benchmarking.
  for(int c = 0; c < COUNT; c++) {
    int b = rand() % BLOCKS;
    lseek(fd, b*SIZE, SEEK_SET);

    for(int e = 0; e < entries; e++) {
      struct timespec t_start, t_stop;
      int buffer;
      clock_gettime(CLOCK_MONOTONIC, &t_start);
      read(fd, &buffer, sizeof(int));
      clock_gettime(CLOCK_MONOTONIC, &t_stop);
      lseek(fd, STEP - sizeof(int), SEEK_CUR);
      table[e][c] = n_sec(&t_start, &t_stop);
    }
  }

  // Collect the statistics and sort them
  printf("#N\tMin\tQ1\tMed\tQ3\tD9\tMax\n");
  for(int e = 0; e < entries; e++)
    qsort(table[e], COUNT, sizeof(long), compare);

  for(int e = 0; e < entries; e++) {
    qsort(table[e], COUNT, sizeof(long), compare);

    long min  = table[e][0];
    long q1   = table[e][COUNT / 4];
    long med  = table[e][COUNT / 2];
    long q3   = table[e][3*(COUNT / 4)];
    long d9   = table[e][9*(COUNT / 10)];
    long max  = table[e][COUNT - 1];
    printf("%d\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", e*STEP, min, q1, med, q3, d9, max);
  }

  return 0;
}
