#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define TAM 1000

sem_t s_cola, s_hambre, s_sed, mutex;
int n_hambre = 0, n_sed = 0;

void * puesto(void * arg){
  int n_pers = *(int *)arg, n = 0;
  while(1){
    n = 0;
    sem_wait(&s_cola);
    if(n_hambre > 0){
      sem_post(&s_hambre);
      printf("Puesto: alimento para superviviente\n");
      fflush(stdout);
      sem_wait(&mutex);
      n_hambre--;
      sem_post(&mutex);
    }else if(n_sed > 0){
      sem_wait(&mutex);
      n_sed--;
      sem_post(&mutex);
      sem_post(&s_sed);
      printf("Puesto: agua para superviviente\n");
      fflush(stdout);
    }
  }
}

void * persona (void * arg){
  int n = *(int *)arg, per = n % 2;
  while(1){
    sem_post(&s_cola);
    fflush(stdout);
    if(per == 0){
      printf("Superviviente %d tiene HAMBRE\n", n);
      fflush(stdout);
      sem_wait(&mutex);
      n_hambre++;
      sem_post(&mutex);
      sem_wait(&s_hambre);
      printf("Superviviente %d recibe COMIDA\n", n);
    }else{
      sem_wait(&mutex);
      n_sed++;
      sem_post(&mutex);
      printf("Superviviente %d tiene SED\n", n);
      fflush(stdout);
      sem_wait(&s_sed);
      printf("Superviviente %d recibe AGUA\n", n);
    }
    sleep(3);
  }
}

int main (int argc, void * argv[]){
  int rc, N = 10;
  void * status;
  if(argc > 1){
    N = atoi(argv[1]);
  }
  int p = N, t[N];
  pthread_t h_puesto, threads[N];
  sem_init(&s_cola, 0, 0);
  sem_init(&s_hambre, 0, 0);
  sem_init(&s_sed, 0, 0);
  sem_init(&mutex, 0, 1);

  rc = pthread_create(&h_puesto, NULL, puesto, (void *)&p);
  if(rc != 0){
    printf("Fallo en pthread_create puesto\n");
    exit(-1);
  }

  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, persona, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en ptrhead_create persona %d", i);
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

