#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

int n = 5, C = 6, n_pers = 0, n_bussiness = 0;
sem_t s_bussiness, s_normal, s_coche, s_salida, mutex;

void * coche (void * arg) {
	while(1){
		n_pers = 0;
		printf("Coche esperando pasajeros\n");fflush(stdout);
		while(n_pers < C){
			sem_wait(&mutex);
			if(n_bussiness > 0){
				sem_post(&s_bussiness);
			}else{
				sem_post(&s_normal);
			}
			n_pers++;
			sem_post(&mutex);
			sem_wait(&s_coche);
		}
		printf("El coche sale a pista. n_pers = %d\n", n_pers);fflush(stdout);
		sleep(5);

		printf("El coche termina la tanda\n\n\n");fflush(stdout);
		for(unsigned k = 0 ; k < C ; k++){
			sem_post(&s_salida);
		}
	}
}

void * p_normal(void * arg){
	int id = *(int *)arg;
	while(1){
		sem_post(&s_coche);
		sem_wait(&s_normal);
		printf("El pasajero con id %d ha entrado en el coche\n", id);fflush(stdout);
		sem_wait(&s_salida);
		printf("El pasajero con id %d ha salido del coche\n", id);fflush(stdout);
		sleep(3);
	}
}

void * p_bussiness (void * arg){
	int id = *(int *)arg;
	while(1){
		sem_wait(&mutex);
		n_bussiness++;
		sem_post(&mutex);
		sem_post(&s_coche);
		sem_wait(&s_bussiness);
		sem_wait(&mutex);
		n_bussiness--;
		sem_post(&mutex);
		printf("El pasajero bussiness %d ha entrado en el coche\n", id);fflush(stdout);
		sem_wait(&s_salida);
		printf("El pasajero bussiness %d ha salido del coche\n", id);fflush(stdout);
		sleep(3);
	}
}

int main (int argc, char * argv[]) {
	if(argc > 1){
		n = atoi(argv[1]);
	}
	int rc = -1, t1[n], t2[n];
	pthread_t h_coche, h_normales[n], h_bussiness[n];
	void * status;

	sem_init(&s_bussiness, 0, 0);
	sem_init(&s_normal, 0, 0);
	sem_init(&s_coche, 0, 0);
	sem_init(&s_salida, 0, 0);
	sem_init(&mutex, 0, 1);

	rc = pthread_create(&h_coche, NULL, coche, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create coche");
		exit(-1);
	}

	for(unsigned i = 0 ; i < n ; i++){
		t1[i] = i;
		rc = pthread_create(&h_normales[i], NULL, p_normal, (void *)&t1[i]);
		if(rc != 0){
			perror("Fallo en pthread_create coche");
			exit(-1);
		}

		t2[i] = i+n;
		rc = pthread_create(&h_bussiness[i], NULL, p_bussiness, (void *)&t2[i]);
		if(rc != 0){
			perror("Fallo en pthread_create coche");
			exit(-1);
		}
	}

	rc = pthread_join(h_coche, &status);
	if(rc != 0){
		perror("Fallo en pthread_join");
		exit(-1);
	}
	return 0;
}
