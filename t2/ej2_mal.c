#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define TAM 10000
int arr[TAM] = {[0 ... TAM-1] = -1};

void * cons(void * arg){
  int n = *(int *)arg;
  while(arr[n]==-1);

  printf("Consumido arr[%d]\n", arr[n]);
}

void * prod(void * arg){
  int n = *(int *)arg;
  if((n < TAM) && arr[n] == -1){
    arr[n] = n;
    printf("producir %d\n", n);

  }else if(arr[n] != -1){
    int pos = n;
    while((pos < TAM) && arr[pos] != -1){
      pos++;
    }
    if(pos < TAM){
      arr[pos] = n;
      printf("%d producido en %d\n", n, pos);
    }else{
      printf("No se pudo producir %d\n", n);
    }
  }
}

int main (int argc, char *argv[]) {
  int nelem=atoi(argv[1]), t[nelem];
  int rc;
  pthread_t threads[2];
  void *status;

  for(unsigned i = 0 ; i < nelem ; i++){
    t[i] = i;
    rc = pthread_create(&threads[0], NULL, prod, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create 0");
      exit(-1);
    }

    rc = pthread_create(&threads[1], NULL, cons, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create 0");
      exit(-1);
    }
  }

  return 0;
}
