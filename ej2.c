#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NTHREADS 2
int v = 0;

void *add100(void * n) {
  for(unsigned i = 0 ; i < 100 ; i++){
    v++;
  }
}

int main (int argc, char * argv[]) {
  int n = atoi(argv[1]);
  pthread_t threads[NTHREADS];
  int t1[n];
  int t2[n];
  int rc = 0;
  void *status;

  for (unsigned i = 0 ; i < n && rc == 0; i++){
    t1[i] = i;
    t2[i] = i;
    rc = pthread_create(&threads[0], NULL, add100, &t1[i]);
    if (rc != 0) {
      printf("ERROR pthread_creeate 0 is %d\n", rc);
      exit(-1);
    }
    rc = pthread_create(&threads[1], NULL, add100, &t2[i]);
    if (rc != 0) {
      printf("ERROR pthread_create 1 is %d\n", rc);
      exit(-1);
    }
  }

  for (unsigned h = 0 ; h < NTHREADS ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %d estado: %ld\n", h, (long)status);
  }

  printf("%d\n", v);
  fflush(stdout);

  return 0;
}
