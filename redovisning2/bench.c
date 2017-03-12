#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define ROUNDS 100
#define LOOP 1000
#define BUFFER 100

int main() {
  void *buffer[BUFFER];
  for(int i = 0; i < BUFFER; i++)
    buffer[i] = NULL;
  void *init = sbrk(0);
  void *current;

  printf("The initial top of the heap is: %p.\n", init);

  for (int j = 0; j < ROUNDS; j++) {
    for (int i = 0; i < LOOP; i++) {
      int index = rand() % BUFFER;
      if (buffer[index] != NULL)
        mfree(buffer[index]);
      size_t size = (size_t)request();
      int *memory;
      memory = mmalloc(size);

      if (memory == NULL) {
        fprintf(stderr, "mmalloc failed\n");
        return(1);
      }
      buffer[index] = memory;

      /* writing to the memory so we know it exists */
      *memory = 123;
    }
    current = sbrk(0);
    int allocated = (int)((current - init) / 1024);
    printf("%d\n", j);
    printf("The current top of the heap is %p.\n", current);
    printf("increased by %d Kbyte\n", allocated);
    /*printf("Internal fragmentation: %d\n", internal_fragmentation());*/
    /*list_len();*/
    printf("Free list length: %d\n", list_len());
    /*printf("The external fragmentation is: %d Kbyte\n", ext_fragmentation() / 1024);*/
  }
}
