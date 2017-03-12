#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void mfree(void *memory);

struct chunk {
  int size;
  struct chunk *next;
};

struct chunk* flist = NULL;

void split(struct chunk *cnk, size_t size) {
  if (cnk->size > size + sizeof(struct chunk)) {
    struct chunk *newCnk = (struct chunk*)((void *)cnk + sizeof(struct chunk) + size);
    newCnk->size = cnk->size - sizeof(struct chunk) - size;
    cnk->size = size;

    mfree((void *)(newCnk + 1));
  }
}

void *mmalloc(size_t size) {
  if (size == 0)
    return NULL;

  struct chunk *next = flist;
  struct chunk *prev = NULL;
  while(next != NULL) {
    if (next->size >= size) {
      if (prev != NULL)
        prev->next = next->next;
      else
        flist = next->next;

      split(next, size);

      return (void *)(next + 1);
    }

    prev = next;
    next = next->next;
  }

  /* use sbrk to allocate new memory */
  void *memory = sbrk(size + sizeof(struct chunk));
  if (memory == (void *)-1)
    return NULL;
  else {
    struct chunk *cnk = (struct chunk*)memory;
    cnk->size = size;

    return (void *)(cnk + 1);
  }
}

void mfree(void *memory) {
  if (memory == NULL)
    return;

  struct chunk *cnk = (struct chunk*)((struct chunk*)memory - 1);

  struct chunk *next = flist;
  struct chunk *prev = NULL;

  while (next != NULL) {
    if (next->size >= cnk->size) {
      if (prev != NULL) {
        prev->next = cnk;
        cnk->next = next;
      } else {
        cnk->next = flist;
        flist = cnk;
      }

      return;
    }

    prev = next;
    next = next->next;
  }

  cnk->next = NULL;
  if (flist == NULL)
    flist = cnk;
  else
    prev->next = cnk;
}

void free_list() {
  struct chunk *ptr = flist;
  while (ptr != NULL) {
    printf("Size: %d\n", ptr->size);
    ptr = ptr->next;
  }
}

unsigned int list_len() {
  unsigned int size = 0;
  struct chunk *ptr = flist;
  while (ptr != NULL) {
    size++;
    ptr = ptr->next;
  }

  return size;
}

unsigned int ext_fragmentation() {
  unsigned int size = 0;
  struct chunk *ptr = flist;
  while (ptr != NULL) {
    size += ptr->size;
    ptr = ptr->next;
  }

  return size;
}
