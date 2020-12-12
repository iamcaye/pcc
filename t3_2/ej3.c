#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t ingredientes, fuma;

int no_hay = -1;

void * fumador (void * arg){
  int n = *(int *)arg;
  while(1){
    pthread_mutex_lock(&mutex);
    while(no_hay != n){
      pthread_cond_wait(&ingredientes, &mutex);
    }
    no_hay = -1;
    pthread_mutex_unlock(&mutex);
    printf("Persona %d fuma\n", n);
    sleep(1);
    printf("Persona %d termina\n", n);
    pthread_cond_signal(&fuma);
  }
}

void * agente (void *arg){
  int p = *(int *)arg;
  while(1){
    pthread_mutex_lock(&mutex);
    no_hay = rand() % p;
    switch(no_hay){
      case 0:
        printf("Agente pone papel y tabaco (1,2)\n");
        break;
      case 1:
        printf("Agente pone papel y cerillas (0,2)\n");
        break;
      case 2:
        printf("Agente pone cerillas y tabaco (0,1)\n");
        break;
    }
    pthread_cond_broadcast(&ingredientes);
    while(no_hay != -1){
      pthread_cond_wait(&fuma, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
}

int main(int argc, char * argv[]){
  int rc, p = 3, t[3];
  pthread_t h_fum[3], h_agente;

  pthread_cond_init(&ingredientes, NULL);
  pthread_cond_init(&fuma, NULL);

  pthread_mutex_init(&mutex, NULL);
  
  rc = pthread_create(&h_agente, NULL, agente, (void *)&p);
  if(rc != 0){
    printf("Fallo en pthread_create agente");
    exit(- 1);
  }

  for(unsigned i = 0 ; i < 3 ; i++){
    t[i] = i;
    rc = pthread_create(&h_fum[i], NULL, fumador, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create fumador %d", i);
      exit(-1);
    }
  }

  void * status;
  for(unsigned k = 0 ; k < 3 ; k++){
    pthread_join(h_fum[k], status);
  }
  return 0;
}
