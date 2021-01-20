#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>
#include <semaphore.h>

#define TAM 100

mqd_t q_soldado[TAM];
int arr[TAM] = {[0 ... TAM-1] = 0};
int M = 10, s_res = 10;
sem_t ciclo;

void * general(void * arg){
  int num = 0, cont = M, res = 0;
  for(unsigned n = 0 ; n < M ; n++){
    do{
      num = rand()%M;
    }while(arr[num] == 1);
    
    cont--;
    printf("El general ha elegido a %d\n", num);
    res = mq_send(q_soldado[num], (char *)&cont, sizeof(int), 0);
    if(res == -1){
      perror("Fallo en mq_send 1");
      exit(-1);
    }
  
    sem_wait(&ciclo);
  }
}

void * soldado(void * arg){
  int id = *(int *)arg, res = 0, num, next = 0;
  while(1){
    res = mq_receive(q_soldado[id], (char *)&num, sizeof(int), 0);
    if(res == -1){
      perror("Fallo en mq_receive soldado");
      exit(-1);
    }
    printf("Soldado %d dice %d\n", id, num);
    if(num == 0 && s_res != 1){
      printf("El soldado %d se salva\n", id);
      arr[id] = 1;
      s_res--;
      sem_post(&ciclo);
      break;
    }else if(s_res == 1){
      printf("Le ha tocado correr al soldado %d", id); 
      break;
    }

    next = id;
    do{
      if(next == M-1){
        next = 0;
      }else{
        next++;
      }
    }while(arr[next] == 1);

    num--;
    res = mq_send(q_soldado[next], (char *)&num, sizeof(int), 0);
    if(res == -1){
      perror("Fallo en mq_receive soldado");
      exit(-1);
    }
  }
}


int main (int argc, char *argv[]) {
  if(argc > 1){
    M = atoi(argv[1]);
  }

  int rc = 0, res = 0;
  pthread_t h_general, h_soldados[M];
  char nombre[20] = {[0 ... 19] = '\0'};

  sem_init(&ciclo, 0, 0);

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 2;
  attr.mq_msgsize = sizeof(int);
  attr.mq_curmsgs = 0;

  for(unsigned j = 0 ; j < M ; j++){
    sprintf(nombre, "/soldado%d", j);
    mq_unlink(nombre);
    q_soldado[j] = mq_open(nombre, O_RDWR|O_CREAT, 0666, &attr);
    if(q_soldado[j] == -1){
      perror("Fallo en mq_open");
      exit(-1);
    }
  }

  rc = pthread_create(&h_general, NULL, general, (void *)&M);
  if(rc != 0){
    printf("Fallo en pthread_create h_general\n");
    exit(-1);
  }

  int t[M];
  for(unsigned i = 0 ; i < M ; i++){
    t[i] = i;
    int rc = pthread_create(&h_soldados[i], NULL, soldado, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create h_soldado %d\n", i);
      exit(-1);
    }
  }

  void *status;
  for(unsigned k = 0 ; k < M ; k++){
    rc = pthread_join(h_soldados[k], status);
    if(rc != 0){
      printf("Fallo en pthread_join\n");
      exit(-1);
    }
  }

  return 0;
}
