#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int N = 0;
pid_t pid;

void * control_entrada (void * arg) {
	while(1){
		sleep(1);
		printf("Entra una persona en la discoteca.\n");
		kill(pid, SIGRTMAX);
	}
}

void * control_salida (void * arg) {
	while(1){
		sleep(2);
		printf("Sale una persona de la discoteca.\n");
		kill(pid, SIGRTMIN);
	}
}

void * gestion (void * arg) {
	int info, rc = -1;
	sigset_t gestion_set;
	sigemptyset(&gestion_set);
	sigaddset(&gestion_set, SIGRTMIN);
	sigaddset(&gestion_set, SIGRTMAX);
	while(1){
		rc = sigwait(&gestion_set, &info);
		if(rc != 0){
			perror("Fallo en sigwait");
			exit(-1);
		}
		
		if(info == SIGRTMIN && N > 0){
			N--;
		}else if(info == SIGRTMAX){
			N++;
		}
		printf("Aforo = %d\n", N);
	}
}

int main (int argc, char * argv[]) {
	pid = getpid();
	int rc = -1;
	pthread_t h_entrada, h_salida, h_gestion;
	void * status;
	sigset_t main_set;

	srand(time(NULL));

	sigemptyset(&main_set);
	sigaddset(&main_set, SIGRTMIN);
	sigaddset(&main_set, SIGRTMAX);
	rc = pthread_sigmask(SIG_BLOCK, &main_set, NULL);
	if(rc != 0){
		perror("Fallo en phtread_sigmask");
		exit(-1);
	}

	rc = pthread_create(&h_gestion, NULL, gestion, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create h_entrada");
		exit(-1);
	}

	rc = pthread_create(&h_entrada, NULL, control_entrada, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create h_entrada");
		exit(-1);
	}

	rc = pthread_create(&h_salida, NULL, control_salida, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create h_salida");
		exit(-1);
	}

	rc = pthread_join(h_gestion, &status);
	if(rc != 0){
		perror("Fallo en pthread_create h_entrada");
		exit(-1);
	}

	rc = pthread_join(h_salida, &status);
	if(rc != 0){
		perror("Fallo en pthread_create h_entrada");
		exit(-1);
	}

	rc = pthread_join(h_entrada, &status);
	if(rc != 0){
		perror("Fallo en pthread_create h_entrada");
		exit(-1);
	}

	return 0;
}
