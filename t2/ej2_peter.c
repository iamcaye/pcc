#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define TAM 100

int turno = 0, nelem = 0;
int f[2] = {0,0};
int elems[TAM];

void * cons (void * arg){
  int i = 0;
  while(1){
    f[0] = 1;
    turno = 1;
    while(turno == 1 && f[1])
      ;
    if(nelem > 0){
      printf("consumido %d\n", i);
      nelem--;
    }
    i++;
    if(i == TAM){
      i = 0;
    }
    f[0] = 0;
  } 
}

void * prod (void * arg){
  int i = 0;
  while(1){
    f[1] = 1;
    turno = 0;
    while(turno == 0 && f[0])
      ;
    if(nelem < TAM){
      nelem++;
      printf("producido %d\n", i);
    }
    i++;
    if(i == TAM){
      i = 0;
    }
    f[1] = 0;
  } 
}

int main (int argc, char * argv[]) {
  int rc, t[2];
  pthread_t threads[2];
  void *status;

  rc = pthread_create(&threads[0], NULL, prod, (void *)&t[0]);
  if(rc != 0){
    perror("Fallo pthread_create 0");
    exit(-1);
  }
  rc = pthread_create(&threads[1], NULL, cons, (void *)&t[1]);
  if(rc != 0){
    perror("Fallo pthread_create 0");
    exit(-1);
  }

  for (long h = 0 ; h < 2 ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }

  return 0;
}
