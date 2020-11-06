#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int v_s[3] = {0,0,0};
sem_t s[3], s_t;

void * trabajador(void * arg){
  while(1){
    for(unsigned i = 0 ; i < 3 ; i++){
      sem_wait(&s[i]);
      printf("sensor %d ha terminado\n", i);
    }
    for(unsigned j = 0 ; j < 3 ; j++){
      printf("El sensor %d ha medido %d\n", j, v_s[j]);
      v_s[j] = 0;
    }
    for(unsigned x = 0 ; x < 3 ; x++){
      sem_post(&s_t);
    }
  }
}

void * sensor(void * arg) {
  int t = *(int *)arg;

  while(1){
    while(v_s[t] != 0)
      ;

    sem_wait(&s_t);
    printf("sensor %d\n", t);

    v_s[t] =( rand() % 6) +2 ;
    sleep(v_s[t]);

    sem_post(&s[t]);
  }
}

int main (int argc, char *argv[]) {
  int rc, t[3], p;
  pthread_t threads[4];
  void *status;
  for(unsigned c = 0 ; c < 3 ; c++){
    sem_init(&s[c], 0, 0);
  }
  sem_init(&s_t, 0, 3);
  
  rc = pthread_create(&threads[0], NULL, trabajador, (void *)&p);
  if(rc != 0){
    perror("Fallo en pthread_create 0");
    exit(-1);
  }

  for(unsigned b = 0 ; b < 3 ; b++) {
    t[b] = b;
    printf("for %d\n", t[b]);
    rc = pthread_create(&threads[b], NULL, sensor, (void *)&t[b]);
    if(rc != 0){
      perror("Fallo en pthread_create 1");
      exit(-1);
    }
  }

  for (long h = 0 ; h < 4 ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }

  return 0;
}
