#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N_SILLAS 5
#define N_CLIENTES 12

sem_t aforo, mutex;
int n_pers = 0, ocupado = 0;

void pidoMesa (int id) {
  do{
    sem_wait(&aforo);
  }while(n_pers >= N_SILLAS || ocupado == 1);
  sem_wait(&mutex);
  n_pers++;
  printf("El cliente %d se ha sentado\n", id);
  if(n_pers >= N_SILLAS && ocupado == 0){
    ocupado = 1;
    printf("Ya se ha completado la mesa. Total = %d\n", n_pers);
  }
  sem_post(&mutex);
}

void meVoy (int id) {
  sem_wait(&mutex);
  n_pers--;
  printf("El cliente %d se va del restaurante\n", id);
  if (n_pers == 0 && ocupado == 1){
    printf("Se ha liberado la mesa\n");
    ocupado = 0;
  }
  sem_post(&aforo);
  sem_post(&mutex);
  sleep(3);
}

void * cliente (void * arg) {
  int id = *(int *)arg;
  while(1){
    pidoMesa(id);
    sleep(rand()%5);
    meVoy(id);
  }
}

int main (int argc, char * argv[]) {
  int rc = -1, t[N_CLIENTES];
  pthread_t h_clientes[N_CLIENTES];
  void * status;
  
  sem_init(&aforo, 0, 5);
  sem_init(&mutex, 0, 1);

  srand(time(NULL));

  for(unsigned i = 0 ; i < N_CLIENTES ; i++){
    t[i] = i;
    rc = pthread_create(&h_clientes[i], NULL, cliente, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create h_clientes");
      exit(-1);
    }
  }

  for(unsigned j = 0 ; j < N_SILLAS ; j++){
    rc = pthread_join(h_clientes[j], status);
    if(rc != 0){
      perror("Fallo en pthread_join");
      exit(-1);
    }
  }
  return 0;
}
