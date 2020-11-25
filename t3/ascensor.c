#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int n_pisos = 5, piso = 0, N = 8, n_pers = 0;
pthread_mutex_t mutex;
pthread_cond_t s_piso[n_pisos];

void * persona(void *arg){
  int n = *(int *)arg, piso_o = rand() % (n_pisos+1), piso_d = rand % (n_pisos+1);
  int he_subido
  while(1){
    printf("Persona %d llama al ascensor desde la planta %d\n", n, piso);
    pthread_mutex_lock(&mutex);
    while(piso_o != piso || n_pers >= 4){
      pthread_cond_wait(&s_piso[n], &mutex);
    }
    printf("Persona %d se ha subido al ascensor\n", n);
    n_pers++;
    while(piso_d != piso){
      pthread_cond_wait(&s_piso[n], &mutex);
    }

    pthread_mutex_unlock(&mutex);
    sleep(1);
  }

}

void * ascensor(void *arg){
  int  dir = 1;
  while(1){
    pthread_mutex_lock(&mutex);
    if(piso == 4){
      dir = -1;
    }else if(piso == 0){
      dir = 1;
    }
    piso += dir;
    pthread_cond_broadcast(&s_piso[piso]);
    pthread_mutex_unlock(&mutex);
  }
}

int main(int argc, char * argv[]){
  if(argc > 1){
    N = atoi(argv[1]);
  }

  int p = N,t[N], rc;
  pthread_t threads[N], h_ascensor;
  pthread_mutex_init(&mutex, NULL);

  for(unsigned j = 0 ; j < n_pisos; j++){
    pthread_cond_init(s_piso[j]);
  }

  rc = pthread_create(&h_ascensor, NULL, ascensor, (void *)&p);
  if(rc != 0){
    printf("Fallo en pthread_create hilo ascensor\n");
    exit(-1);
  }

  for(unsigned i = 0 ; i < N ; i++){
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, persona, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create hilo ascensor\n");
      exit(-1);
    }
  }

  void * status;
  for(unsigned h = 0 ; h < N ; h++){
    pthread_join(threads[h], status);
  }

  return 0;
}
