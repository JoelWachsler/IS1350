#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

int main(int argc, char *argv[]) {
  int done = 0;
  ucontext_t one;
  ucontext_t two;

  getcontext(&one);

  printf("hello\n");

  if (!done) {
    done = 1;
    swapcontext(&two, &one);
  }

  return 0;
}
