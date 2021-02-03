#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5
#define C 6

int n_pers = 0, n_bussiness = 0, corriendo = 0;
pthread_cond_t c_coche, c_salida, c_bussiness, c_normal;
pthread_mutex_t mutex;

void * coche (void * arg){
	while(1){
		n_pers = 0;
		pthread_mutex_lock(&mutex);
		printf("Coche esperando a pasajeros\n");
		while(n_pers < C){
			pthread_cond_wait(&c_coche, &mutex);
			if(n_bussiness > 0){
				n_bussiness--;
				pthread_cond_signal(&c_bussiness);
			}else{
				pthread_cond_signal(&c_normal);
			}
		}
		pthread_mutex_unlock(&mutex);
		printf("El coche sale a pista\n");
		sleep(5);
		printf("El coche termina\n");
		pthread_cond_broadcast(&c_salida);
	}
}

void * p_normal (void * arg) {
	int id = *(int *)arg;
	while(1){
		pthread_mutex_lock(&mutex);
		while(n_pers >= C || n_bussiness > 0){
			pthread_cond_wait(&c_normal, &mutex);
		}
		printf("El pasajero bussiness %d se ha subido al coche\n", id);
		n_pers++;
		pthread_cond_signal(&c_coche);
		pthread_cond_wait(&c_salida, &mutex);
		pthread_mutex_unlock(&mutex);
		printf("Pasajero normal %d sale del coche\n", id);
		sleep(3);
	}
}

void * p_bussiness (void * arg) {
	int id = *(int *)arg;
	while(1){
		pthread_mutex_lock(&mutex);
		n_bussiness++;
		while(n_pers >= C){
			pthread_cond_wait(&c_bussiness, &mutex);
		}
		printf("El pasajero bussiness %d se ha subido al coche\n", id);
		n_pers++;
		pthread_cond_signal(&c_coche);
		pthread_cond_wait(&c_salida, &mutex);
		pthread_mutex_unlock(&mutex);
		printf("Pasajero bussiness %d sale del coche\n", id);
		sleep(3);
	}
}

int main (int argc, char * argv[]) {
	int rc = -1, t1[N], t2[N];
	pthread_t h_coche, h_normales[N], h_bussiness[N];
	void * status;

	pthread_cond_init(&c_bussiness, 0);
	pthread_cond_init(&c_salida, 0);
	pthread_cond_init(&c_normal, 0);
	pthread_cond_init(&c_coche, 0);
	pthread_mutex_init(&mutex, 0);

	rc = pthread_create(&h_coche, NULL, coche, NULL);
	if(rc != 0){
		perror("Fallo en pthread_create coche");
		exit(-1);
	}

	for(unsigned i = 0 ; i < N ; i++){
		t1[i] = i;
		rc = pthread_create(&h_normales[i], NULL, p_normal, (void *)&t1[i]);
		if(rc != 0){
			perror("Fallo en pthread_create coche");
			exit(-1);
		}

		t2[i] = i+N;
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
