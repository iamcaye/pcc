#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TAM 100

int N = 10;
mqd_t der[TAM], izq[TAM];

void comprobarMM (int num, int *mm){
  //printf("Num: %d\t mm = {%d, %d}\n", num, mm[0], mm[1]);
	if(num < mm[0]){
		mm[0] = num;
	}else if(mm[1] < num){
		mm[1] = num;
	}
}

void * hebra (void * arg){
	int id = *(int *)arg, num, leido = 0, mm[2] ={0,0};
	
	num = rand() % 100;
  printf("ID = %d\t num = %d\n", id, num);

	if(id == 0){
		mm[0] = num;
		mm[1] = num;
    //printf("Hilo %d manda mensaje a la derecha\n", id);
		leido = mq_send(der[id], (char *)&mm, sizeof(mm), 0);
		if(leido < 0){
			perror("Fallo en mq_send 1");
			exit(-1);
		}
	}else{
    //printf("Hilo %d espera mensaje de la hebra de su izquierda(der[id-1])\n", id);
		leido = mq_receive(der[id-1], (char *)&mm, sizeof(mm), 0);
		if(leido < 0){
			perror("Fallo en mq_receive");
			exit(-1);
		}
		comprobarMM(num, mm);

    //printf("Hilo %d manda mensaje a la der\n", id);
		leido = mq_send(der[id], (char *)&mm, sizeof(mm), 0);
		if(leido < 0){
			perror("Fallo en mq_send 2");
			exit(-1);
		}
	}

  if(id != N-1){
    //printf("Hilo %d manda mensaje a su izquierda\n", id);
		leido = mq_receive(izq[id+1], (char *)&mm, sizeof(mm), 0);
		if(leido < 0){
			perror("Fallo en mq_receive");
			exit(-1);
		}
  }

	if(id != 0){
    //printf("Hilo %d manda mensaje a su izquierda\n", id);
		leido = mq_send(izq[id], (char *)&mm, sizeof(mm), 0);
		if(leido < 0){
			perror("Fallo en mq_receive");
			exit(-1);
		}
	}

	printf("[Hilo %d] max = %d, min = %d\n", id, mm[1], mm[0]);
}

int main (int argc, char *argv[]) {
	int rc;
	pthread_t threads[TAM];
	char nombre[100] = {[0 ... 99] = '\0'};
	if(argc > 1){
		N = atoi(argv[1]);
	}

	srand(time(NULL));

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 2*sizeof(int);
	attr.mq_curmsgs = 0;

	int t[N];
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
