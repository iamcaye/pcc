#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NTHREADS 2
int v = 3;

void *add100(void * n) {
  v = v+100;
  printf("%d\n", v);
}

int main (int argc, void * argv[]) {
  int n = atoi(argv[1]);
  pthread_t threads[n];
  int t[n];
  int rc;

  for (unsigned i = 0 ; i < n ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, add100, &t[i]);
  }

  pthread_exit(NULL);
  return 0;
}
