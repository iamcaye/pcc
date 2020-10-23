#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define TAM 1000
int arr[TAM] = {[0 ... TAM-1] = -1};

void * func (void * arg){
  int pos = *(int *)arg;
  printf("pos %d\n", pos);
  if(pos < 2){
    arr[pos] = pos;
  }else{
    while((arr[pos-1] < 0) && (arr[pos-2] < 0)){}

    arr[pos] = arr[pos-1] + arr[pos-2];
  }
}

int main (int argc, char *argv[]) {
  int n = atoi(argv[1]), t[n], rc;
  pthread_t threads[n];
  void * status;

  for(int i = 0 ; i < n ; i++) {
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, func, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create");
      exit(-1);
    }
  }

  for (int h = 0 ; h < n ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %d estado: %ld\n", h, (long)status);
  }

  for(int j = 0 ; j < n ; j++) {
    printf("<%d---%d>\n", j,arr[j]);
  }

  return 0;
}
