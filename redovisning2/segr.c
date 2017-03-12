#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define PAGE_SIZE 4096

size_t internal_frag = 0;

void mfree(void *memory);

struct node {
  int size;
  struct node *next;
};

/**
 * Bytes:  Index:
 * 16      0
 * 32      1
 * 64      2
 * 128     3
 * 256     4
 * 512     5
 * 1024    6
 * 2048    7
 * 4096    8
 */
struct node *free_mem[9] = { NULL };

struct node* voidToNode(void* mem) {
  return (struct node*)((int*)mem - 1);
}

void* nodeToVoid(struct node* node) {
  return (void*)((int*)node + 1);
}

struct node* extract_node(int index) {
  struct node* node = (struct node*)free_mem[index];
  free_mem[index] = node->next;

  return node;
}

void split(int index) {
  struct node* node = extract_node(index);
  node->size--;
  struct node* split = (struct node*)((void*)node + (int)(16<<node->size));
  split->size = node->size;

  mfree(nodeToVoid(node));
  mfree(nodeToVoid(split));
}

void *mmalloc(size_t size) {
  if (size == 0)
    return NULL;

  for (int i = 4; i <= 12; i++) {
    if (size + sizeof(unsigned int) <= 1<<i) {
      // Start from i and go up until we find a chunk greater or
      // equal to the one we wanted
      for (int j = i; j <= 12; j++) {
        if (free_mem[j - 4] != NULL) {
          // Chunk size is greater -> let's split it until we get the
          // desired size defined by i
          if (i != j)
            for (; j > i; j--)
              split(j - 4);

          internal_frag += (int)(16<<free_mem[i - 4]->size) - size;
          return nodeToVoid(extract_node(i - 4));
        }
      }

      // Found the correct size but there are no mfree chunks to use
      break;
    }
  }

  /* use sbrk to allocate new memory */
  void *memory = sbrk(4096);
  if (memory == (void *)-1)
    return NULL;
  else {
    struct node *node = (struct node*)memory;
    // 2^12 = 4096
    // But index 0 is 16 bytes = 2^4
    // -> 12 - 4 = 8
    node->size = 12-4;
    // Just to be sure
    node->next = NULL;

    mfree(nodeToVoid(node));

    return mmalloc(size);
  }
}

void mfree(void *memory) {
  if (memory == NULL)
    return;

  // Go one int back to get the struct
  struct node *node = voidToNode(memory);
  node->next = free_mem[node->size];
  free_mem[node->size] = node;
}

/*void free_list() {*/
  /*struct chunk *ptr = flist;*/
  /*while (ptr != NULL) {*/
    /*printf("Size: %d\n", ptr->size);*/
    /*ptr = ptr->next;*/
  /*}*/
/*}*/

/*unsigned int ext_fragmentation() {*/
  /*unsigned int size = 0;*/
  /*struct chunk *ptr = flist;*/
  /*while (ptr != NULL) {*/
    /*size += ptr->size;*/
    /*ptr = ptr->next;*/
  /*}*/
  /*return size;*/
/*}*/

void list_len() {
  for (int i = 0; i < 9; ++i) {
    int counter = 0;
    struct node* ptr = free_mem[i];
    while (ptr != NULL) {
      counter++;
      ptr = ptr->next;
    }

    printf("Bytes: %d, Length: %d\n", (int)(16 << i), counter);
  }
}

unsigned int internal_fragmentation() {
  unsigned int num = (unsigned int)(internal_frag / 1024);
  internal_frag = 0;
  return num;
}

/*int main(int argc, char *argv[]) {*/
  /*int* p = mmalloc(10);*/
  /**p = 123;*/
  /*printf("%d\n", *p);*/
  /*mfree(p);*/

  /*return 0;*/
/*}*/
