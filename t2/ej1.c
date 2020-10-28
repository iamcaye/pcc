#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define TAM 1000
long arr[TAM] = {[0 ... TAM-1] = -1};

void * func (void * arg){
  long pos = *(long *)arg;
  printf("pos %ld\n", pos);
  if(pos < 2){
    arr[pos] = pos;
  }else{
    while((arr[pos-1] < 0) || (arr[pos-2] < 0)){}

    arr[pos] = arr[pos-1] + arr[pos-2];
  }
}

int main (int argc, char *argv[]) {
  long n = atoi(argv[1]), t[n+1];
  int rc;
  pthread_t threads[n+1];
  void * status;

  for(long i = 0 ; i <= n ; i++) {
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, func, (void *)&t[i]);
    if(rc != 0){
      perror("Fallo en pthread_create");
      exit(-1);
    }
  }

  for (long h = 0 ; h < n ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }

  for(long j = 0 ; j <= n ; j++) {
    printf("<%ld---%ld>\n", j,arr[j]);
  }

  return 0;
}
