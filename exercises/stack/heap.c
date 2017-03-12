#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int pid = getpid();

  long *heap = (unsigned long*)calloc(40, sizeof(unsigned long));

  free(heap);

  printf("heap[2]: 0x%lx\n", heap[2]);
  printf("heap[1]: 0x%lx\n", heap[1]);
  printf("heap[0]: 0x%lx\n", heap[0]);
  printf("heap[-1]: 0x%lx\n", heap[-1]);
  printf("heap[-2]: 0x%lx\n", heap[-2]);

  char command[50];
  sprintf(command, "cat /proc/%d/maps \n\n", pid);
  system(command);

  return 0;
}
