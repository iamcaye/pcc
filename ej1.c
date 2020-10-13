#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NTHREADS 3

struct arg_struct{
  int n;
  char *c;
};

void *printChar (void * argum){
  struct arg_struct *args = (struct arg_struct *)argum;
  for (unsigned j = 0 ; j < args -> n ; j++) {
    printf("%s\t", args -> c);
  }
}

int main (int argc, void * argv[]) {
  pthread_t threads[NTHREADS];
  struct arg_struct args[NTHREADS];
  int rc;

  for (unsigned i = 0 ; i < NTHREADS ; i++) {
    args[i].n = atoi(argv[1]);
    args[i].c = argv[i+2];
    rc = pthread_create(&threads[i], NULL, printChar, (void *)&args[i]);
  }

  pthread_exit(NULL);
  return 0;
}
