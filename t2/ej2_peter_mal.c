#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define TAM 100
int arr[TAM] = {[0 ... TAM-1] = -1};
int turno;
int f[2]={0,0};

void * cons(void * arg){
  int n = *(int *)arg, i = 0, j = 0;
  while(1){
    f[0] = 1;
    turno = 1;
    while(f[1] && turno == 1)
      ;
    while(i < n){
      while(arr[j]==-1){};
      printf("Consumido arr[%d] = %d\n", j, arr[j]);
      arr[j]= -1;
      j++;
      if(j == TAM){
        j = 0;
      }
      i++;
    }
    f[0] = 0;
  }
}

void * prod(void * arg){
  int n = *(int *)arg, pos=0, i = 0;
  while(1){
    f[1] = 1;
    turno = 0;
    while(f[0] && turno == 0)
      ;
    while(i < n){
      if(arr[pos] == -1){
        arr[pos] = i;
        printf("producir %d\n", i);
    
      }else{
        while(arr[pos] != -1){
          pos++;
          if(pos == TAM){
            pos = 0;
          }
        }
        arr[pos] = i;
        printf("%d producido en %d\n", i, pos);
      }
      i++;
    }
    f[1] = 0;

  }
}

int main (int argc, char *argv[]) {
  int nelem=atoi(argv[1]), t[2];
  int rc;
  pthread_t threads[2];
  void *status;


  t[0] = nelem;
  t[1] = nelem;
  rc = pthread_create(&threads[0], NULL, prod, (void *)&t[0]);
  if(rc != 0){
    perror("Fallo en pthread_create 0");
    exit(-1);
  }
  rc = pthread_create(&threads[1], NULL, cons, (void *)&t[1]);
  if(rc != 0){
    perror("Fallo en pthread_create 0");
    exit(-1);
  }

  for (long h = 0 ; h < 2 ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }
  return 0;
}
