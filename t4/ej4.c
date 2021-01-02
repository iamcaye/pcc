#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <mqueue.h>

/*
 Se podria implementar con una lista enlazada para ahorrar memoria...
 */

#define TAM 100

int N = 10;
mqd_t canal[TAM];

int get_min(int * arr, int id){
  int tmp = arr[0];
  for(unsigned i = 0 ; i < N-id ; i++){
    if(arr[i] < tmp){
      tmp = arr[i];
    }
  }

  return tmp;
}

void * generador (void * arg) {
  int arr[TAM], res;
  for(unsigned l = 0 ; l < N ; l++){
    arr[l] = rand()%100;
    res = mq_send(canal[0], (char *)&arr[l], sizeof(int), 0);
    if(res == -1){
      perror("Fallo en mq_send generador");
      exit(-1);
    }
  }
}

void * hebra (void * arg){
  int id = *(int *)arg, res = 0, arr[TAM] = {[0 ... TAM-1] = 100}, ret = 0, control = 0;
  for(unsigned j = 0 ; j < N-id ; j++){
   //implementar algortimo para ordenar el array pero sin mandarlo entero 
   res = mq_receive(canal[id], (char *)&arr[j], sizeof(int), 0);
   if(res == -1){
     perror("Fallo en mq_receive 1");
     exit(-1);
   }
   //printf("[Hilo %d]: arr[%d] = %d\n", id, j, arr[j]);
  }

  ret = get_min(arr, id);
  for(unsigned k = 0 ; k < N-id ; k++){
    if(ret == arr[k] && control == 0){
      control = 1;
      continue;
    }
    res = mq_send(canal[id+1], (char *)&arr[k], sizeof(int), 0);
    if(res == -1){
      perror("Fallo en mq_send 1");
      exit(-1);
    }
  }
  printf("HILO %d: %d\n", id, ret);
}

int main (int argc, char * argv[]) {
  if(argc > 1){
    N = atoi(argv[1]);
  }

  char nombre[TAM] = {[0 ... TAM-1] = '\0'};
  int t[N], rc;
  
  srand(time(NULL));

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 2;
	attr.mq_msgsize = sizeof(int);
	attr.mq_curmsgs = 0;

  for(unsigned j = 0 ; j < N ; j++){
    sprintf(nombre, "/canal%d", j);
    mq_unlink(nombre);
		canal[j] = mq_open(nombre, O_RDWR|O_CREAT, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH), &attr);
    if(canal[j] == -1){
      perror("Fallo en mq_open canal");
      exit(-1);
    }
  }

  pthread_t threads[TAM], h_gen;

  rc = pthread_create(&h_gen, NULL, generador, (void *)&N);
  if(rc != 0){
    perror("Fallo en pthread_init generador");
    exit(-1);
  }

  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, hebra, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_init generador");
      exit(-1);
    }
  }

  void * status;
  for(unsigned k = 0 ; k < N ; k++){
    rc = pthread_join(threads[k], status);
    if(rc != 0){
      printf("Fallo en pthread_join\n");
      exit(-1);
    }
  }
  return 0;
}
