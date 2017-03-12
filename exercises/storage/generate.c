/*
 * =====================================================================================
 *
 *       Filename:  generate.c
 *
 *    Description:  Generates dummy files
 *
 *        Version:  1.0
 *        Created:  2017-02-21 19:09:33
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
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>

#define BLOCKS 1024*1024
#define SIZE 512

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: generate <file name>\n");
    return -1;
  }
  char *name = argv[1];

  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int flag = O_RDWR | O_CREAT;
  int fd = open(name, flag, mode);
  assert(fd != -1);

  int buffer;
  for(int b = 0; b < BLOCKS; b++) {
    lseek(fd, SIZE * b, SEEK_SET);
    buffer = b;
    write(fd, &buffer, sizeof(int));
  }
  close(fd);
  printf("done\n");

  return 0;
}
