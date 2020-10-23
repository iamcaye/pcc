#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

void *sort(void * arg){
  int * arr = (int *)arg;
  int l = sizeof(arr)/sizeof(arr[0]);
  printf("l = %d\n",l);
  int tmp;

  for(unsigned j = 0 ; j < l ; j++) {
    printf("%d -- %d\t", j, arr[j]);
    for(unsigned k = 1 ; k < l ; k++) {
      if(arr[j] > arr[k]) {
        tmp = arr[k];
        arr[k] = arr[j];
        arr[j] = tmp;
      }
    }
  }
}

int main (int argc, char * argv[]) {
  int p = atoi(argv[1]);
  int n = atoi(argv[2]);
  pthread_t threads[p];
  int arr[n];
  int rc;
  void * status;

  for (unsigned i = 3 ; i < argc ; i++){
    arr[i-3] = atoi(argv[i]);
  }

  int l = n/p;

  if(l > 0){
    int sarr[p][l];
    int i = 0;
    for(unsigned x = 0 ; x < p ; x++){
      for(unsigned y = 0 ; y < l ; y++){
        sarr[x][y] = arr[i];
        i++;
      }
    }

    for (unsigned h = 0 ; h < p ; h++) {
      rc = pthread_create(&threads[h], NULL, sort, (void *)&sarr[h]);
      if(rc != 0){
        perror("Fallo en pthread_create");
        exit(1);
      }
    }

    for (unsigned z = 0 ; z < p ; z++) {
      rc = pthread_join(threads[z], &status);
      if (rc != 0) {
        printf("ERROR pthread_join() is %d\n", rc);
        exit(-1);
      }
      printf("Fin thread %d estado: %ld\n", z, (long)status);
    }
  
    for(unsigned u = 0 ; u < p ; u++){
      for(unsigned v = 0 ; v < l ; v++){
        printf("%d\t", sarr[u][v]);
      }
    }
  }

  return 0;
}
