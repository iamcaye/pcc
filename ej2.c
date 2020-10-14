#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NTHREADS 2
int v = 3;

void *add100(void * n) {
  v = v+100;
  pthread_exit(NULL);
}

int main (int argc, void * argv[]) {
  int n = atoi(argv[1]);
  pthread_t threads[n];
  int t[n];
  int rc = 0;
  void *status;

  for (unsigned i = 0 ; i < n && rc == 0; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, add100, &t[i]);
    printf("%d --- %d\n", i, v);
  }

  for (unsigned h = 0 ; h < n ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %d estado: %ld\n", h, (long)status);
  }
  return 0;
}
