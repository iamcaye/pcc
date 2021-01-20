#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int arr[100]= {[0 ... 99] = 0};
sem_t s_c[3], s_cola;

void * colocador(void * arg){
  int c = *(int *)arg, cont = 0;
  while(1){
    sem_wait(&s_cola);
    while(arr[cont] != 0){
      cont++;
      if(cont == c){
        cont = 0;
      }
    }
    arr[cont] = (rand() % 2) +1;
    printf("ID Producto %d\n", arr[cont]);
    sem_post(&s_c[arr[cont]]);
  }
}

void * robot (void * arg){
  int t = *(int *)arg, n = 0;
  while(1){
    printf("%d intenta recoger\n", t);
    sem_wait(&s_c[t]);
    while(arr[n] != t){
      n++;
      if(n >= 100){
        n = 0;
      }
    }
    printf("Robot %d retira %d\n", t, arr[n]);
    arr[n] = 0;
    sem_post(&s_cola);
  }
}

int main (int argc, char *argv[]){
  pthread_t threads[4];
  int rc, c = 100, t[3];
  void *status;

  for(unsigned k =  0 ; k < 3 ; k++){
    sem_init(&s_c[k], 0, 0);
  }

  if(argc > 1) {
    c = atoi(argv[1]);
  }
  sem_init(&s_cola, 0, c);


  for(unsigned i = 1 ; i < 4 ; i++){
    t[i-1] = i-1;
    printf("creado %d\n", t[i-1]);
    rc = pthread_create(&threads[i], NULL, robot, (void *)&t[i-1]);
    if(rc != 0){
      perror("Fallo en pthread_create colocador");
      exit(-1);
    }
  }

  rc = pthread_create(&threads[0], NULL, colocador, (void *)&c);
  if(rc != 0){
    perror("Fallo en pthread_create colocador");
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
}
