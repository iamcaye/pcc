#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void * cons(void * arg){
  int n 
}

int main (int argc, char *argv[]) {
  int n=atoi(arg[1]), t[n], rc;
  pthread_t threads[2];
  void *status;

  for(unsigned i = 0 ; i < n ; i++){
    rc = pthread_create(&threads[0], NULL, prod, &t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create 0");
      exit(-1);
    }

    rc = pthread_create(&threads[1], NULL, cons, &t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create 0");
      exit(-1);
    }
  }

  return 0;
}
