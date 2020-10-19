#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *SayHi (void *threadid) {
  long tid = (*(long *)threadid);
  printf("Hola estamos aprendiendo a usar la libreria pthread de POSIX\n");
  printf("Soy %ld\n", tid);
}

int main (int argc, void * argv[]) {
  pthread_t thread[5];
  long t[5];
  int rc;

  for (unsigned i = 0 ; i < 5 ; i++) {
    t[i] = i;
    rc = pthread_create(&thread[i], NULL, SayHi, (void *)&t[i]);
    if(rc < 0){
      printf("Fallo en pthread_create %d", i);
      exit(0);
    }
  }
  pthread_exit(NULL);
  return 0;
}
