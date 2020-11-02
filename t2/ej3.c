#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

long nagua = 0;
int f1[2] = {0,0},f2[2] = {0,0};
int t1 = 0, t2 = 0, cant1 = -1, cant2 = -1;

void * rio (void * arg){
  while(1 && cant1 != 0){
    f1[0] = 1;
    t1 = 1;
    while(t1 == 1 && f1[1])
      ;
    f2[0] = 1;
    t2 = 1;
    while(t2 == 1 && f2[1])
      ;
    nagua += 1000;
    cant1--;
    printf("Sube el agua: %ldL \n", nagua);
    fflush(stdout);

    f1[0] = 0;
    f2[0] = 0;
  }
} 

void * presa (void * arg){
  while(1 && cant2 != 0){
    f1[1] = 1;
    t1 = 0;
    while(t1 == 0 && f1[0])
      ;
    f2[1] = 1;
    t2 = 0;
    while(t2 == 0 && f2[0])
      ;

    cant2--;
    if(nagua > 0){
      nagua -= 1000;
      printf("Baja el agua: %ldL \n", nagua);
      fflush(stdout);
    }else{
      printf("Presa vacia\n");
      fflush(stdout);
    } 

    f1[1] = 0;
    f2[1] = 0;
  }
} 

int main (int argc, char *argv[]) {
  pthread_t threads[4];
  int rc, t[4];
  void *status;
  if(argc > 1){
    cant1 = atoi(argv[1]);
    cant2 =  atoi(argv[1]);
  }

  rc = pthread_create(&threads[0], NULL, rio, (void *)&t[0]);
  if(rc != 0){
    perror("fallo en pthread_create 0");
    exit(-1);
  }
  rc = pthread_create(&threads[1], NULL, rio, (void *)&t[1]);
  if(rc != 0){
    perror("fallo en pthread_create 1");
    exit(-1);
  }
  rc = pthread_create(&threads[2], NULL, presa, (void *)&t[2]);
  if(rc != 0){
    perror("fallo en pthread_create 2");
    exit(-1);
  }
  rc = pthread_create(&threads[3], NULL, presa, (void *)&t[3]);
  if(rc != 0){
    perror("fallo en pthread_create 3");
    exit(-1);
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
