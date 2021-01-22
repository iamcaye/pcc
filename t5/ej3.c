#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#define N_COCHES 10
#define MAX_COCHES 8

sem_t barrera;

void * controlador (void *arg) {
	while(1){
	}
}

void * coche (void * arg) {
	int id = *(int *)arg, tipo = 0;
	while(1){
		tipo = rand()%2;
		sem_wait(&barrera);
	}
}

int main (int argc, char * argv[]){
	int rc = -1, t[N_COCHES];
	sigset_t main_set;
	pthread_t h_coches[N_COCHES], h_control;
	void * status;

	sem_init(&barrera, 0, 0);

	sigemptyset(&main_set);
	sigaddset(&main_set, SIGRTMAX);
	sigaddset(&main_set, SIGRTMIN);
	sigaddset(&main_set, SIGUSR1);

	rc = pthread_sigmask(SIG_BLOCK, &main_set, NULL);
	if(rc != 0){
		perror("Fallo en pthread_sigmask");
		exit(-1);
	}

	rc = pthread_create(&h_control, NULL, controlador, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create controlador");
		exit(-1);
	}

	for(unsigned i = 0 ; i < N_COCHES ; i++){
		t[i] = i;
		rc = pthread_create(&h_coches[i], NULL, conche, (void *)&t[i]);
		if(rc != 0){
			perror("Fallo en pthread_create coche");
			exit(-1);
		}
	}

	for(unsigned k = 0 ; k < N_COCHES ; k++){
		rc = pthread_join(h_coches[i], &status);
		if(rc != 0){
			perror("Fallo en pthread_join coche");
			exit(-1);
		}
	}

	return 0;
}
