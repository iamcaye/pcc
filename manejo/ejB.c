#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex;
pthread_cond_t c_prod, c_cons;
int cantidad = 0;

void * consumidor (void * arg){
	int n = *(int *)arg;
	while(1){
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&c_cons, &mutex);
		if(cantidad > 0){
			cantidad--;
			printf("Consume productos.\tTotal: %d\n", cantidad);
		}
		pthread_mutex_unlock(&mutex);
	}
}

void * productor (void * arg){
	int n = *(int *)arg;
	while(1){
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&c_prod, &mutex);
		cantidad++;
		pthread_mutex_unlock(&mutex);
		printf("Produce productos.\tTotal: %d\n", cantidad);
	}
}

void * controlador (void * arg) {
	int n = *(int *)arg, cont = 0;
	while(1){
		struct timespec next, period;

		if(clock_gettime(CLOCK_MONOTONIC, &next) != 0){
			printf("Fallo en clock_gettime\n");
			exit(-1);
		}

		period.tv_nsec = 0;
		period.tv_sec = 1;

		while(1){
			if(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0) != 0){
				printf("Fallo en clock_nanosleep\n");
				exit(-1);
			}
			pthread_cond_signal(&c_prod);
			cont++;
			if(cont == 3){
				pthread_cond_signal(&c_cons);
				cont = 0;
			}

			next.tv_sec = next.tv_sec + period.tv_sec;
			printf("Periodo!\n");
		}
		
	}
}

int main (int argc, char * argv[]) {
	int rc, N = 10;
	
	if(argc > 1){
		N  = atoi(argv[1]);
	}
	rc = pthread_cond_init(&c_prod, NULL);
	if(rc != 0){
		printf("Error en pthread_cond_init c_prod\n");
		exit(-1);
	}

	rc = pthread_cond_init(&c_cons, NULL);
	if(rc != 0){
		printf("Error en pthread_cond_init c_cons\n");
		exit(-1);
	}

	rc = pthread_mutex_init(&mutex, NULL);
	if(rc != 0){
		printf("Error en pthread_mutex_init\n");
		exit(-1);
	}

	pthread_t cons, prod, control;
	rc = pthread_create(&prod, NULL, productor, (void *)&N);
	if(rc != 0){
		printf("Error en pthread_create prod\n");
		exit(-1);
	}

	rc = pthread_create(&cons, NULL, consumidor, (void *)&N);
	if(rc != 0){
		printf("Error en pthread_create cons\n");
		exit(-1);
	}

	rc = pthread_create(&control, NULL, controlador, (void *)&N);
	if(rc != 0){
		printf("Error en pthread_create cons\n");
		exit(-1);
	}

	void * status;
	rc = pthread_join(prod, status);
	if(rc != 0){
		printf("Error en pthread_join prod\n");
		exit(-1);
	}

	rc = pthread_join(cons, status);
	if(rc != 0){
		printf("Error en pthread_join cons\n");
		exit(-1);
	}

	return 0;
}
