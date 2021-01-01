#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <mqueue.h>

#define TAM 100

sem_t mutex;
int N = 10;
mqd_t izq[TAM], der[TAM];

// Implementar alogoritmo para contar las veces que se repite mi numero
void * hebra (void * arg) {
  int id = *(int *)arg;
  int arr[TAM] = {[0 ... TAM-1] = 0}, res = 0;

  int num = rand()%100;

  if(id == 0){
    arr[id] = num;
    //printf("Hilo %d manda mensaje a su derecha\n", id);
    res = mq_send(der[id], (char *)&arr, sizeof(arr),0);
    if(res == -1){
      perror("Fallo en mq_send 1");
      exit(-1);
    }
  }else{
    //printf("Hilo %d espera mensaje desde su izq(der[id-1])\n", id);
    res = mq_receive(der[id-1], (char *)&arr, sizeof(arr),0);
    if(res == -1){
      perror("Fallo en mq_receive 1");
      exit(-1);
    }

    arr[id] = num;

    if(id != N-1){
      //printf("Hilo %d manda mensaje a su derecha\n", id);
      res = mq_send(der[id], (char *)&arr, sizeof(arr),0);
      if(res == -1){
        perror("Fallo en mq_send 2");
        exit(-1);
      }
    }
  }

  if(id == N-1){
    //printf("Hilo %d manda mensaje a su izq\n", id);
    res = mq_send(izq[id], (char *)&arr, sizeof(arr),0);
    if(res == -1){
      perror("Fallo en mq_send 3");
      exit(-1);
    }
  }else{
    //printf("Hilo %d espera mensaje desde su der(izq[id+1])\n", id);
    res = mq_receive(izq[id+1], (char *)&arr, sizeof(arr),0);
    if(res == -1){
      perror("Fallo en mq_receive 2");
      exit(-1);
    }

    if(id != 0){
      //printf("Hilo %d manda mensaje a su izquierda\n", id);
      res = mq_send(izq[id], (char *)&arr, sizeof(arr),0);
      if(res == -1){
        perror("Fallo en mq_send 4");
        exit(-1);
      }
    }
  }

  int ret[TAM] = {[0 ... TAM-1] = 0}, cont = 0;
  for(unsigned l = 0 ; l < N ; l++){
    if(l != id && num == arr[l]){
      ret[cont] = l;
      cont++;
    }
  }

  sem_wait(&mutex);
  if(cont == 0){
    printf("[Hilo %d](%d) no se repite\n", id, num);
  }else{
    printf("[Hilo %d](%d) se repite %d veces en: ", id, num, cont);
    for(unsigned m = 0 ; m < cont ; m++){
      printf("%d, ", ret[m]);
    }
    printf("\n");
  }
  sem_post(&mutex);


}

int main (int argc, char * argv[]) {
  if(argc > 1){
    N = atoi(argv[1]);
  }

  int t[N], rc;
  char nombre[TAM] = {[0 ... TAM-1] = '\0'};
  pthread_t threads[TAM];

  srand(time(NULL));

  sem_init(&mutex, 0, 1);

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 2;
	attr.mq_msgsize = TAM*sizeof(int);
	attr.mq_curmsgs = 0;


  for(unsigned j = 0 ; j < N ; j++){
		sprintf(nombre, "/der%d", j);
		mq_unlink(nombre);
		der[j] = mq_open(nombre, O_RDWR|O_CREAT, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH), &attr);
		if(der[j] == -1)  {
			perror("Fallo en mq_open der");
			exit(-1);
		}

		sprintf(nombre, "/izq%d", j);
		mq_unlink(nombre);
		izq[j] = mq_open(nombre, O_RDWR|O_CREAT, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH), &attr);
		if(izq[j] == -1)  {
			perror("Fallo en mq_open izq");
			exit(-1);
		}
  }

	for(unsigned i = 0; i < N ; i++){
		t[i] = i;
		rc = pthread_create(&threads[i], NULL, hebra, (void *)&t[i]);
		if(rc != 0){
			perror("Fallo en pthread_create");
			exit(-1);
		}
	}

	void * status;
	for(unsigned k = 0 ; k < N ; k++){
		rc = pthread_join(threads[k], status);
		if(rc != 0){
			perror("Fallo en pthread_create");
			exit(-1);
		}
	}

  return 0;
}
