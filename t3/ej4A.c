#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t s_der, s_izq;
int n_der = 0, n_izq = 0;

void * coche(void * arg){
  int n_coche = *(int *)arg;
  int dir = rand() %2;
  while(1){
    if(dir == 0){
      printf("Coche %d esperando a la derecha\n", n_coche);
      sem_wait(&s_der);
      printf("Coche %d ha pasado por la derecha\n", n_coche);
      sem_post(&s_izq);
    }else{
      printf("Coche %d esperando a la izquierda\n", n_coche);
      sem_wait(&s_izq);
      printf("Coche %d ha pasado por la izquierda\n", n_coche);
      sem_post(&s_der);
    }
  }
}

int main (int argc, char * argv[]){
  int rc, N = 10, t[N];
  void * status;
  if(argc > 1){
    N = atoi(argv[1]);
  }
  pthread_t threads[N];
  sem_init(&s_der, 0, 1);
  sem_init(&s_izq, 0, 0);

  for(unsigned i = 0 ; i < N ; i++) {
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, coche, (void *)&t[i]);
    if(rc != 0) {
      printf("Fallo en pthread_create %d", i);
      exit(-1);
    }
  }

  for (long h = 0 ; h < N+2 ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }

  return 0;
}
