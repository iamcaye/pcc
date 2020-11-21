#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int n_pers = 0, N = 10, ha_viajado = 0;
sem_t mutex;

void * helicoptero(void *arg){
  int p = *(int *)arg;
  while(1){
    if(n_pers < p){
      ha_viajado = 0;
      printf("Helicoptero se va a curar\n");
    }else{
      printf("Helicoptero salva personas\n");
      sem_wait(&mutex);
      n_pers -= 10;
      ha_viajado = 1;
      sem_post(&mutex);
    }
    sleep(2);
  }
}

void * persona (void * arg) {
  int n = *(int *)arg, viajo = 0;

  while(1){
    if(!viajo){
      sem_wait(&mutex);
      if(n_pers < N){
        viajo = 1;
        printf("Superviviente %d viaja! ---> N_pers = %d\n", n, n_pers);
        n_pers++;
      }
      sem_post(&mutex);
    }else if(ha_viajado == 1){
      viajo = 0;
    }
  }
}

int main(int argc, char * argv[]){
  if(argc > 1){
    N = atoi(argv[1]);
  }
  int rc, t[2*N];
  void * status;

  sem_init(&mutex, 0,1);
  pthread_t threads[2*N], t_heli;
  int p = N;

  rc = pthread_create(&t_heli, NULL, helicoptero, (void *)&p);
  if(rc != 0){
    printf("Fallo en pthread_create helicoptero\n");
    exit(-1);
  }

  for(unsigned i = 0 ; i < 2*N ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, persona, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create helicoptero\n");
      exit(-1);
    }
  }

  for (long h = 0 ; h < 2*N ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }

  return 0;
}
