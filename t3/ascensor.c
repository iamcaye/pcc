#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 5

int n_pisos = 5, piso = 0, N = 8, n_pers = 0;
pthread_mutex_t mutex;
pthread_cond_t s_piso;

void * persona(void *arg){
  int n = *(int *)arg, piso_o = (rand() % (n_pisos+1)), piso_d = (rand() % (n_pisos+1));
  int he_subido, cont = 0, cont1 = 0;
  while(1){
    printf("Persona %d llama al ascensor desde la planta %d\t Destino: %d\n", n, piso_o, piso_d);
    pthread_mutex_lock(&mutex);

    while(piso_o != piso || n_pers >= 4){
      pthread_cond_wait(&s_piso, &mutex);
    }
    n_pers++;
    printf("Persona %d se ha subido al ascensor \tTotal: %d\n", n, n_pers);
    
    while(piso_d != piso){
      pthread_cond_wait(&s_piso, &mutex);
    }
    printf("Persona %d se ha bajado al ascensor\tTotal: %d\n", n, n_pers);
    n_pers--;

    pthread_mutex_unlock(&mutex);
    sleep(1);
  }

}

void * ascensor(void *arg){
  int  dir = 1, cont = 0;
  while(1){
    pthread_mutex_lock(&mutex);
    piso=piso+dir;
    
    if(piso == 5){
      dir= -1;
    }else if(piso == 0){
      dir = 1;
    }

    pthread_cond_broadcast(&s_piso);
    printf("ASCENSOR EN PISO %d\n", piso);
    pthread_mutex_unlock(&mutex);
    sleep(1);
  }
}

int main(int argc, char * argv[]){
  if(argc > 1){
    N = atoi(argv[1]);
  }

  int p = N,t[N], rc;
  pthread_t threads[N], h_ascensor;
  pthread_mutex_init(&mutex, NULL);

  pthread_cond_init(&s_piso, NULL);

  rc = pthread_create(&h_ascensor, NULL, ascensor, (void *)&p);
  if(rc != 0){
    printf("Fallo en pthread_create hilo ascensor\n");
    exit(-1);
  }

  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, persona, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create hilo persona\n");
      exit(-1);
    }
  }

  void * status;
  for(unsigned h = 0 ; h < N ; h++){
    pthread_join(threads[h], status);
  }

  return 0;
}
