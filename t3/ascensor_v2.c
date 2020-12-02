#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;

int main(int argc, char * argv[]){
  if(argc > 1){
    N = atoi(argv[1]);
  }

  int p = N,t[N], rc;
  pthread_t threads[N], h_ascensor;
  pthread_mutex_init(&mutex, NULL);

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
