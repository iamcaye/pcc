#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define TAM 128

struct st_args {
  pthread_t t;
  char * buff;
};

void *func() {
  sleep(2);
}

void *getter(void * th){
  pthread_t thread = (pthread_t)th;
  int rc;
  char *buff;
  rc = pthread_getname_np(&thread, buff, TAM);
  if(rc != 0){
    perror("pthread_getname");
    exit(0);
  }
  printf("---- %s\t", buff);
}

int v = 0;

int main (int argc, void * argv[]) {
  pthread_t threads[2];
  char buff[TAM];
  char n[TAM];
  int t[2];
  int rc;
  struct st_args args[100];

  rc = pthread_create(&threads[0], NULL, func, &t[0]);
  if(rc != 0){
    perror("pthread_create 0");
    exit(0);
  }

  for (unsigned i = 0 ; i < 100 ; i++) {
    sprintf(n, "%d", i);
    rc = pthread_setname_np(threads[0], n);
    if(rc != 0){
      perror("pthread_setname 1");
      exit(0);
    }

    rc = pthread_create(&threads[1], NULL, getter, (void *)&threads[0]);
    if(rc != 0){
      perror("pthread_create 1");
      exit(0);
    }
  }

  pthread_exit(NULL);
  return 0;
}
