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
  printf("\n");
}

int main (int argc, void * argv[]) {
  pthread_t threads[NTHREADS];
  struct arg_struct args[NTHREADS];
  void * status;
  int rc;

  for (unsigned i = 0 ; i < NTHREADS ; i++) {
    args[i].n = atoi(argv[1]);
    args[i].c = argv[i+2];
    rc = pthread_create(&threads[i], NULL, printChar, (void *)&args[i]);
  }

  for (unsigned h = 0 ; h < NTHREADS ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %d estado: %ld\n", h, (long)status);
  }
  return 0;
}
