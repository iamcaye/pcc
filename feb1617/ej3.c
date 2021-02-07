#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5
#define C 6

pthread_cond_t c_coche, c_salida;
pthread_mutex_t mutex;

int pista = 1, n_pers = 0, n_bussiness;

int subirCocheN (int id) {
  printf("Pasajero normal %d esperando para entrar en el coche\n", id);
  while(n_pers > C && n_bussiness > 0){
    pthread_cond_wait(&c_coche, &mutex);
  }
  n_pers++;
  printf("EL pasajero normal %d se ha subido al coche. Total = %d\n", id, n_pers);
  if(n_pers == C){
    return 1;
  }else{
    return 0;
  }
}

int subirCocheB (int id) {
  printf("Pasajero bussiness %d esperando para entrar en el coche\n", id);
  while(n_pers > C){
    n_bussiness++;
    pthread_cond_wait(&c_coche, &mutex);
  }
  n_pers++;
  n_bussiness--;
  printf("EL pasajero bussiness %d se ha subido al coche. Total = %d\n", id, n_pers);
  if(n_pers == C){
    return 1;
  }else{
    return 0;
  }
}

void * cocheEnMarcha (int id) {
  printf("El pasajero %d es el conductor\nEl coche sale a pista\n\n\n", id);
  sleep(5);
  printf("El coche termina la tanda\n");
  n_pers = 0;
  pthread_cond_broadcast(&c_salida);
}

void * p_normal (void * arg) {
  int id = *(int *)arg, conductor = -1;;
  while(1){
    pthread_mutex_lock(&mutex);
    conductor = subirCocheN(id);
    if(conductor == 1){
      cocheEnMarcha(id);
    }else{
      pthread_cond_wait(&c_salida, &mutex);
    }
    printf("El pasajero normal %d se ha bajado del coche. Total = %d\n", id, n_pers);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&c_coche);
    sleep(3);
  }
}

void * p_bussiness (void * arg) {
  int id = *(int *)arg, conductor = -1;
  while(1){
    pista = 1;
    pthread_mutex_lock(&mutex);
    conductor = subirCocheB(id);
    if(conductor == 1){
      cocheEnMarcha(id);
    }else{
      pthread_cond_wait(&c_salida, &mutex);
    }
    printf("El pasajero bussiness %d se ha bajado del coche. Total = %d\n", id, n_pers);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&c_coche);
    sleep(3);
  }
}

int main (int argc, char * argv[]) {
  int rc, t1[N], t2[N];
  pthread_t h_normal[N], h_bussiness[N];
  void * status;

  rc = pthread_cond_init(&c_coche, 0);
  if(rc != 0){
    perror("Fallo en pthread_cond_init");
    exit(-1);
  }

  rc = pthread_cond_init(&c_salida, 0);
  if(rc != 0){
    perror("Fallo en pthread_cond_init");
    exit(-1);
  }

  for(unsigned i = 0 ; i < N ; i++){
    t1[i] = i;
    rc = pthread_create(&h_normal[i], NULL, p_normal, (void *)&t1[i]);
    if(rc != 0){
      perror("Fallo en pthread_create normal");
      exit(-1);
    }

    t2[i] = i+N;
    rc = pthread_create(&h_bussiness[i], NULL, p_bussiness, (void *)&t2[i]);
    if(rc != 0){
      perror("Fallo en pthread_create bussiness");
      exit(-1);
    }
  }

  for(unsigned j = 0 ; j < N ; j++){
    rc = pthread_join(h_normal[j], status);
    if(rc != 0){
      perror("Fallo en pthread_join normal");
      exit(-1);
    }

    rc = pthread_join(h_bussiness[j], status);
    if(rc != 0){
      perror("Fallo en pthread_join bussiness");
      exit(-1);
    }
  }

  return 0;
}
