#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int n_pers = 0;
sem_t mutex, s_heli;

void * helicoptero(void * arg){
  int per = *(int *)arg;
  while(1){
    if(n_pers < per){
      printf("Helicoptero ayuda a los enfermeros\n");
      sleep(1);
    }else{
      printf("Helicoptero recoge supervivientes\n");
      for(unsigned j = 0 ; j < per ; j++){
        sem_post(&s_heli);
        sem_wait(&mutex);
        n_pers--;
        sem_post(&mutex);
      }
    }
  }
}

void * persona(void * arg){
  int n = *(int *)arg;
  while(1){
    sleep(1);
    sem_wait(&mutex);
    printf("Superviviente %d esperando\n", n);
    n_pers++;
    sem_post(&mutex);
    sem_wait(&s_heli);
    printf("Superviviente %d recogido\n", n);
  }
}

int main (int argc, char *argv[]) {
  void *status;
  int rc, N = 10;
  if(argc > 1){
    N = atoi(argv[0]);
  }
  int p = N, t[2*N];

  pthread_t threads[2*N], thread_hel;
  sem_init(&mutex, 0, 1);
  sem_init(&s_heli, 0, 0);

  rc = pthread_create(&thread_hel, NULL, helicoptero, (void *)&p);
  if(rc != 0){
    printf("Fallo en pthread_create helicoptero\n");
    exit(-1);
  }

  for(unsigned i = 0; i < 2*N ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, persona, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create persona %d\n", i);
      exit(-1);
    }
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
