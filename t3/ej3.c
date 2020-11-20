#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/*
 *  TODO:
 *  - Quitar espera activa y solo usar semafoross
 */

sem_t s_sur, s_norte, mutex, s_cola_sur, s_cola_norte;

void * sur (void * arg){
  int N = *(int *)arg, n = 0;
  while(1){
    printf("Sur comprobando aire\n");
    sem_wait(&s_cola_sur);
    sem_wait(&mutex);
    sem_post(&s_sur);
    printf("Sur permite aterrizaje\n");
    sem_post(&mutex);
  }
}

void * norte (void * arg){
  int N = *(int *)arg, n = 0;
  while(1){
    printf("Norte comprobando aire\n");
    sem_wait(&s_cola_norte);
    sem_wait(&mutex);
    sem_post(&s_norte);
    printf("Norte permite aterrizaje\n");
    sem_post(&mutex);
  }
}

void * avion(void * arg){
  int av = *(int *)arg, dir = av%2;
  while(1){
    if(dir == 0){
      printf("Avion %d solicita permiso para aterrizar desde el norte\n", av);
      sem_post(&s_cola_norte);
      sem_wait(&s_norte);
      printf("Avion %d ha aterrizado desde el norte\n", av);
    }else{
      printf("Avion %d solicita permiso para aterrizar desde el sur\n", av);
      sem_post(&s_cola_sur);
      sem_wait(&s_sur);
      printf("Avion %d ha aterrizado desde el sur\n", av);
    }
    
  }
}

int main(int argc, char * argv[]) {
  int N = 10, rc, t[N];
  if(argc > 1) {
    N = atoi(argv[1]);
  }
  pthread_t threads[N+2];
  sem_init(&mutex, 0,1);
  sem_init(&s_sur, 0,0);
  sem_init(&s_norte, 0,0);
  sem_init(&s_cola_norte, 0, 0);
  sem_init(&s_cola_sur, 0, 0);
  void *status;

  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    printf("Avion %d despega\n", i);
    rc = pthread_create(&threads[i+2], NULL, avion, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create avion");
      exit(-1);
    }
  }

  rc = pthread_create(&threads[0], NULL, norte, (void *)&N);
  if(rc != 0){
    perror("Fallo en pthread_create norte");
    exit(-1);
  }

  rc = pthread_create(&threads[1], NULL, sur, (void *)&N);
  if(rc != 0){
    perror("Fallo en pthread_create sur");
    exit(-1);
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
