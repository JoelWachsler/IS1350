#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int size = atoi(argv[1]);

  long *block = malloc(size);

  printf("magic: 0x%lx\n", block[-1])

  return 0;
}
