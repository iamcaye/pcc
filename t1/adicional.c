#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define TAM 10000000
double arr[TAM] = {[0 ... TAM-1] = 0};

void * func (void * arg) {
  long pos = (*(long *)arg);
  long u;
  if(pos % 2 == 0){
    u = 0;
    //printf("%ld paaaaar?\n", pos);
  }else{
    u = 1;
    //printf("%ld impaaaaar?\n", pos);
  }
  double res = pos, n = 0;
  for(long v = u; v < pos ; v=v+2) {
    //printf("%ld = u\t%f\n" , v, arr[v]);
    while(arr[v] == 0 && v != 0);
    res = res + arr[v];
    n++;
  }
  if(n != 0){
    arr[pos] = res/n;
  }else{
    arr[pos] = res;
  }
  //printf("<%ld -- %f>\n",pos,arr[pos]);
  
}

int main (int argc, char * argv[]) {
  int n = atoi(argv[1]), rc;
  long t[n];
  pthread_t threads[n];
  void * status;

  for (unsigned i = 0 ; i < n ; i++) {
    t[i] = i;
    rc = pthread_create(&threads[i], NULL, func, (void *)&t[i]);
    if(rc != 0) {
      printf("Fallo en pthread_create %d\n", i);
      exit(-1);
    }
  }

  for (unsigned h = 0 ; h < n ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %d estado: %ld\n", h, (long)status);
  }

  for(unsigned k = 0 ; k < n ; k++){
    printf("%d --- %f\n", k, arr[k]);
  }

  return 0;
}
