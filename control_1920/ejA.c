#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TAM 100

int N = 5, arr[TAM] = {[0 ... TAM-1] = 0}, n_regalos = 0;
pthread_mutex_t mutex;
pthread_cond_t c_elfos[TAM], c_control;

int comprobar_elfos(){
  int res = 0, tmp = arr[0];
  for(unsigned j = 0; j < N ; j++){
    if(tmp > arr[j]){
      res = j;
      tmp = arr[j];
    }
  }
  return res;
}

void * santa (void * arg) {
  int N = *(int *)arg;
  while(1){
    printf("SANTA: preparando regalo\n");
    sleep(rand()%2);
    pthread_mutex_lock(&mutex);
    n_regalos++;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&c_control);
  }
}

void * controlador (void * arg) {
  int N = *(int *)arg, id = 0;
  while(1){
    pthread_mutex_lock(&mutex);
    while(n_regalos == 0){
      printf("CONTROLADOR: esperando regalos\n");
      pthread_cond_wait(&c_control, &mutex);
    }

    printf("CONTROLADOR: recibe regalos\n");
    id = comprobar_elfos();
    arr[id]++;
    n_regalos--;
    pthread_mutex_unlock(&mutex);
    printf("CONTROLADOR: entrega el regalo al elfo %d\n", id);
    pthread_cond_signal(&c_elfos[id]);
  }
}

void * elfo (void * arg){
  int id = *(int *)arg;
  while(1){
    pthread_mutex_lock(&mutex);
    while(arr[id] == 0){
      printf("ELFO %d: esperando regalos\n", id);
      pthread_cond_wait(&c_elfos[id], &mutex);
    }
    printf("ELFO %d: recibe regalos\tTotal = %d\n", id, arr[id]);
    arr[id]--;
    printf("ELFO %d: reparte regalo\tTotal = %d\n", id, arr[id]);
    pthread_mutex_unlock(&mutex);
  }
}

int main (int argc, char * argv[]) {
  int rc = 0;
  if(argc > 1){
    N = atoi(argv[1]);
  }

  pthread_t h_santa, h_elfos[N], h_control;
  pthread_mutex_init(&mutex, NULL);

  rc = pthread_create(&h_santa, NULL, santa, (void *)&N);
  if(rc != 0){
    printf("Fallo en pthread_create santa\n");
    exit(-1);
  }

  rc = pthread_create(&h_control, NULL, controlador, (void *)&N);
  if(rc != 0){
    printf("Fallo en pthread_create controlador\n");
    exit(-1);
  }

  int t[N];
  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    rc = pthread_create(&h_elfos[i], NULL, elfo, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create controlador\n");
      exit(-1);
    }
  }

  void *status;
  for(unsigned k = 0 ; k < N ; k++){
    rc = pthread_join(h_elfos[k], status);
    if(rc != 0) {
      printf("Fallo en pthread_join\n");
      exit(-1);
    }
  }

  return 0;
}
