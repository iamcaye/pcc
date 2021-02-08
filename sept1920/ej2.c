#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N 5

char * sentidos[] = {"izq", "der"};
int n_coches[2] = {0, 0}, ocupado = 1, pasando = 0;
int n_emergencias[2] = {0, 0};
pthread_cond_t c_dir[2], c_puente, c_salida;
pthread_mutex_t mutex;

void cambio_dir(int n){
  if(n == 0){
    ocupado = 1;
  }else{
    ocupado = 0;
  }
}

void * coche (void * arg) {
  int id = *(int *)arg, dir = rand()%2, emergencia = 0;
  
  while(1){
    emergencia = rand()%4;
    pthread_mutex_lock(&mutex);
    if(emergencia == 1){
      n_emergencias[dir]++;
    }
    while(ocupado != dir){
      pthread_cond_wait(&c_puente, &mutex);
    }

    n_coches[dir]++;

    while((pasando != 0 && n_emergencias[dir] != 0) && emergencia != 1 ){
      pthread_cond_wait(&c_dir[dir], &mutex);
    }
    pasando = 1;
    if(emergencia == 1){
      printf("ENTRA LA AMBULANCIA DESDE LA %s\n", sentidos[dir]);
    }else{
      printf("El coche %d entra al puente desde la %s. Total = %d\n", id, sentidos[dir], n_coches[dir]);
    }
    pthread_cond_signal(&c_dir[dir]);
    pthread_mutex_unlock(&mutex);
    sleep(1);
    pthread_mutex_lock(&mutex);
    n_coches[dir]--;
    pasando = 0;
    if(emergencia == 1){
      printf("SALE LA AMBULANCIA DESDE LA %s\n", sentidos[dir]);
      n_emergencias[dir]--;
    }else{
      printf("El coche %d ha pasado desde la %s. Total = %d\n", id, sentidos[dir], n_coches[dir]);
    }
    if(n_coches[dir] == 0){
      printf("\nCAMBIO DE SENTIDO\n");
      cambio_dir(dir);
      pthread_cond_broadcast(&c_puente);
    }else{
      pthread_cond_signal(&c_dir[dir]);
    }
    
    pthread_mutex_unlock(&mutex);
    sleep(rand()%10);
  }
}

int main (int argc, char * argv[]) {
  int rc = -1, t[2*N];
  pthread_t h_coches[2*N];
  void * status;

  srand(time(NULL));

  rc = pthread_cond_init(&c_salida, 0);
  if(rc != 0){
    perror("Fallo en pthread_cond_init c_dir");
    exit(-1);
  }

  rc = pthread_cond_init(&c_puente, 0);
  if(rc != 0){
    perror("Fallo en pthread_cond_init c_dir");
    exit(-1);
  }

  for(unsigned k = 0 ; k < 2 ; k++){
    rc = pthread_cond_init(&c_dir[k], 0);
    if(rc != 0){
      perror("Fallo en pthread_cond_init c_dir");
      exit(-1);
    }
  }

  for(unsigned i = 0 ; i < 2*N ; i++){
    t[i] = i;
    rc = pthread_create(&h_coches[i], NULL, coche, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create coche\n");

    }
  }

  for (unsigned j = 0 ; j < 2*N ; j++) {
    rc = pthread_join(h_coches[j], status);
    if(rc != 0){
      perror("Fallo en pthread_join");
      exit(-1);
    }
  }
  return 0;
}
