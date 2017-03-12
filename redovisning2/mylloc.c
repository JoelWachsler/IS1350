#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct chunk {
  int size;
  struct chunk *next;
};

struct chunk* flist = NULL;

void *malloc(size_t size) {
  if (size == 0)
    return NULL;

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

void free(void *memory) {
  if (memory == NULL)
    return;

  // Using insertion sort to insert the freed chunk

  struct chunk *cnk = (struct chunk*)((struct chunk*)memory - 1);
  cnk->next = NULL;

  // Base cases:
  // list is empty -> set this chunk as the first element
  // only one item exists -> insert this element before or after it
  // the first element is greater than the current one -> insert this element before the first one
  if (flist == NULL) {
    flist = cnk;

    return;
  }

  if (flist->next == NULL) {
    if (flist->size >= cnk->size) {
      cnk->next = flist;
      flist = cnk;
    } else
      flist->next = cnk;

    return;
  }

  if (flist->size >= cnk->size) {
    cnk->next = flist;
    flist = cnk;

    return;
  }

  // Current pointer in list
  struct chunk *ptr = flist;
  while (ptr->next != NULL && ptr->next->size < cnk->size)
    ptr = ptr->next;

  if (ptr->next == NULL) {
    ptr->next = cnk;
  } else {
    cnk->next = ptr->next;
    ptr->next = cnk;
  }
}

void free_list() {
  struct chunk *ptr = flist;
  while (ptr != NULL) {
    printf("Size: %d\n", ptr->size);
    ptr = ptr->next;
  }
}
