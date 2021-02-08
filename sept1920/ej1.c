#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define C 3 // Capacidad del camion
#define N 2 // Numero de camiones

sem_t s_cargar, s_camion;

void * operario (void * arg) {
  int n_contenedor = 0;
  while(1){
    if(n_contenedor < C){
      n_contenedor++;
      printf("El operario carga el contenedor. Total = %d\n", n_contenedor);
    }else{
      sem_wait(&s_camion);
      printf("El operario manda el contendor al camion\n");
      n_contenedor = 0;
      sem_post(&s_cargar);
    }
    sleep(1);
  }
}

void * camion (void * arg) {
  int id = *(int *)arg;
  while(1){
    sem_wait(&s_cargar);
    printf("El camion %d se va a vaciar el contenedor\n", id);
    sleep(3);
    sem_post(&s_camion);
  }
}

int main (int argc, char * argv[]) {
  int rc = -1, t[N];
  void * status;
  pthread_t h_operario, h_camion[N];

  sem_init(&s_cargar, 0, 0);
  sem_init(&s_camion, 0, 2);

  rc = pthread_create(&h_operario, NULL, operario, NULL);
  if(rc != 0){
    perror("Fallo en pthread_create operario\n");
    exit(-1);
  }
  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    rc = pthread_create(&h_camion[i], NULL, camion, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create operario\n");
      exit(-1);
    }
  }

  for(unsigned j = 0 ; j < N ; j++){
    rc = pthread_join(h_camion[j], status);
    if(rc != 0){
      perror("Fallo en pthread_join");
      exit(-1);
    }
  }
  return 0;
}
