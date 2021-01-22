#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int total = 0;

void * controlador (void *arg) {
	int n = 0;
	while(1){
		n = rand()%2;
		if(n == 0){
			printf("El controlador despierta al consumidor\n");
			kill(getpid(), SIGRTMIN);
		}else{
			printf("El controlador despierta al productor\n");
			kill(getpid(), SIGRTMAX);
		}
		sleep(1);
	}
}

void * consumidor (void * arg) {
	sigset_t consum_set;
	sigemptyset(&consum_set);
	sigaddset(&consum_set, SIGRTMIN);
	int res = -1, info = 0;

	while(1){
		printf("Consumidor dormido\n");
		res = sigwait(&consum_set, &info);
		if(res == -1){
			perror("Fallo en sigwait consumidor");
			exit(-1);
		}
		if(total <= 0){
			printf("Consumidor no tiene nada para consumir\n");
		}else{
			total--;
			printf("Consumidor hace su trabajo -> Total = %d\n", total);
		}
	}
}

void * productor (void * arg) {
	sigset_t prod_set;
	sigemptyset(&prod_set);
	sigaddset(&prod_set, SIGRTMAX);
	int res = -1, info = 0;

	while(1){
		printf("Productor dormido\n");
		res = sigwait(&prod_set, &info);
		if(res == -1){
			perror("Fallo en sigwait productor");
			exit(-1);
		}
		total++;
		printf("Productor hace su trabajo -> Total = %d\n", total);
	}
}

int main (int argc, void * argv[]) {
	int rc = -1;
	pthread_t h_consum, h_prod, h_control;
	void *status;
	srand(time(NULL));

	sigset_t main_set;
	sigemptyset(&main_set);
	sigaddset(&main_set, SIGRTMIN);
	sigaddset(&main_set, SIGRTMAX);
	pthread_sigmask(SIG_BLOCK, &main_set, NULL);

	rc = pthread_create(&h_control, NULL, controlador, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create controlador");
		exit(-1);
	}

	rc = pthread_create(&h_prod, NULL, productor, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create controlador");
		exit(-1);
	}

	rc = pthread_create(&h_consum, NULL, consumidor, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create controlador");
		exit(-1);
	}

	if(pthread_join(h_control, &status) != 0){
		perror("Fallo en pthread_join");
		exit(-1);
	}
	return 0;
}
