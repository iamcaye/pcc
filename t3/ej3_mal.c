#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TAM 1000
int arr[TAM] = {[0 ... TAM-1] = -1};
int turno = 0;
int f[2] = {0,0};
sem_t s_sur, s_norte;

void * sur (void * arg){
  int N = *(int *)arg, k = 0;
  while(1){
    sem_wait(&s_sur);
    f[0] = 1;
    turno = 1;
    printf("SUR comprobando pista\n");
    while(turno == 1 && f[1])
      ;
    while(arr[k] != 1){
      k++;
      if(k == N){
        k = 0;
      }
    }
    arr[k] = -1;
    printf("Avion %d ha aterrizado desde el SUR\n", k);
    f[0] = 0;
  }
}

void * norte (void * arg){
  int N = *(int *)arg, k = 0;
  while(1){
    sem_wait(&s_norte);
    f[1] = 1;
    turno = 0;
    printf("NORTE comprobando pista\n");
    while(turno == 0 && f[0])
      ;
    while(arr[k] != 0){
      k++;
      if(k == N){
        k = 0;
      }
    }
    arr[k] = -1;
    printf("Avion %d ha aterrizado desde el NORTE\n", k);
    f[1] = 0;
  }
}

void * avion(void * arg){
  int av = *(int *)arg;
  while(1){
    while(arr[av] != -1)
      ;
    arr[av] = rand() % 2;
    if(arr[av] == 0){
      printf("Avion %d solicita permiso para aterrrizar desde el NORTE\n", av);
      sem_post(&s_norte);
    }else{
      printf("Avion %d solicita permiso para aterrrizar desde el SUR\n", av);
      sem_post(&s_sur);
    }
  }
}

int main(int argc, char * argv[]) {
  int N = 10, rc, t[N];
  if(argc > 1) {
    N = atoi(argv[1]);
  }
  pthread_t threads[N+2];
  sem_init(&s_sur, 0,0);
  sem_init(&s_norte, 0,0);
  void *status;

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

  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    printf("Avion %d despega\n", i);
    rc = pthread_create(&threads[i+2], NULL, avion, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create avion");
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
