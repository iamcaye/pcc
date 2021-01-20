#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#define N_COCHES 10
pid_t mipid = -1;
sem_t barrera;

void * coche (void * arg) {
	int id = *(int *)arg, info = 0, res = 0;
	unsigned n = 0;
	sigset_t set_coche;

	sigemptyset(&set_coche);
	sigaddset(&set_coche, SIGRTMIN);
	while(1){
		sem_wait(&barrera);
		printf("El coche %d llega a la cola\n", id);
		
		res = kill(mipid, SIGRTMAX);
		if(res != 0){
			perror("Fallo en kill 1");
			exit(-1);
		}

		printf("El coche %d esta esperando en la cola\n", id);
		res = sigwait(&set_coche, &info);
		if(res != 0){
			perror("Fallo en sigwait 1");
			exit(-1);
		}
		printf("El coche %d pasa al aparcamiento\n", id);
	}
}

void * controlador (void * arg) {
	sigset_t set_control;
	int res = 0, info = 0;

	sigemptyset(&set_control);
	sigaddset(&set_control, SIGRTMAX);

	while(1){
		printf("Controlador esperando coches\n");
		sem_post(&barrera);
		res = sigwait(&set_control, &info);
		if(res != 0){
			perror("Fallo en sigwait controlador");
			exit(-1);
		}

		printf("Controlador deja pasar al coche\n");
		res = kill(mipid, SIGRTMIN);
		if(res != 0){
			perror("Fallo en kill controlador");
			exit(-1);
		}

		sleep(1);
	}
}

int main (int argc, char * argv[]) {
	int rc = 0, t[N_COCHES];
	pthread_t h_control, h_coches[N_COCHES];
	sigset_t sigset;
	void *status;

	sem_init(&barrera, 0,0);

	mipid = getpid();

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGRTMIN);
	sigaddset(&sigset, SIGRTMAX);
	pthread_sigmask(SIG_BLOCK, &sigset, NULL);

	rc = pthread_create(&h_control, NULL, controlador, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create h_control");
		exit(-1);
	}

	for(unsigned i = 0 ; i < N_COCHES ; i++){
		t[i] = i;
		rc = pthread_create(&h_coches[i], NULL, coche, (void *)&t[i]);
		if(rc != 0){
			perror("Fallo en pthread_create h_control");
			exit(-1);
		}
	}

	for(unsigned k = 0 ; k < N_COCHES ; k++){
		rc = pthread_join(h_coches[k], status);
		if(rc != 0){
			perror("Fallo en pthread_join");
			exit(-1);
		}
	}
	return 0;
}
