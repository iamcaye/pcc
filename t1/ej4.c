#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

void * sort (void * arg) {
  int * arr = (int *)arg;
  int l = sizeof(arr)/sizeof(arr[0])+1, tmp;
  for (unsigned i = 0 ; i < l ; i++){
    for (unsigned k = 1 ; k < l ; k++){
      if(arr[i] < arr[k]){
        arr[i] = tmp;
        arr[i] = arr[k];
        arr[k] = tmp;
      }
    }
  }
}

int max (int a, int b) {
  if(a > b){
    return a;
  }
  return b;
}

int main (int argc, char * argv[]) {
  int m = atoi(argv[1]), n = atoi(argv[2]), l = n/m, rc, res = 0;
  int sarr[m][l];
  pthread_t threads[m];
  void *status;
  
  int u = 3;
  for (unsigned i = 0 ; i < m ; i++) {
    for(unsigned j = 0 ; j < l ; j++) {
      sarr[i][j] = atoi(argv[u]);
      u++;
    }
  }

  for(unsigned k = 0 ; k < m ; k++) {
    rc = pthread_create(&threads[k], NULL, sort, (void *)&sarr[k]);
    if(rc != 0) {
      perror("Fallo en pthread_create");
      exit(-1);
    }
  }

  for (unsigned h = 0 ; h < m ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %d estado: %ld\n", h, (long)status);
  }

  for (unsigned i = 0 ; i < m ; i++) {
    res = max(res, sarr[i][0]);
    printf("%d --- %d\n", res, sarr[i][0]);
  }

  printf("El maximo es %d\n", res);
  return 0;
}
