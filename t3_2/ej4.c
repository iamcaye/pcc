#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NRECURSOS 20
#define TAM 100

int N = 10, arr[TAM] ={[0 ... TAM-1] = -1};
int turno = 0, n_rec = 0;
int proc = 0;
pthread_mutex_t mutex;
pthread_cond_t c_control[TAM];
int cont = 0;

void qrecursos (int n, int rec) {
	pthread_mutex_lock(&mutex);
	printf("%d < %d", cont, turno);
	while((arr[cont] != -1)){
		cont++;
		if(cont == TAM){
			cont = 0;
		}
	}
	arr[cont] = n;
	printf("Proceso %d necesita %d recursos.Pos(%d) Recursos libres: %d\n", n, rec, cont, (NRECURSOS-n_rec));

	while((arr[turno] != n) || ((n_rec + rec) > NRECURSOS)){
		pthread_cond_wait(&c_control[n], &mutex);
	}
	turno++;
	if(turno == TAM){
		turno = 0;
	}
	n_rec += rec;
	pthread_mutex_unlock(&mutex);
}

void librecursos(int n, int rec) {
	int c = 0;
	printf("Proceso %d consumiendo recursos...\n", n);
	sleep(1);
	pthread_mutex_lock(&mutex);
	n_rec -= rec;
	while(arr[c] != n){
		c++;
		if(c == TAM)
			c = 0;
	}
	arr[c] = -1;
	pthread_mutex_unlock(&mutex);
	printf("Proceso %d ha terminado\n", n);
	pthread_cond_signal(&c_control[arr[turno]]);
}

void * proceso (void * arg) {
	int n = *(int *)arg, rec = (rand() % NRECURSOS);
	while(1){
		qrecursos(n, rec);
		librecursos(n,rec);
		sleep(2);
	}
}

		// MAIN 
int main (int argc, char * argv[]){
  if((argc > 1) && (atoi(argv[1]) < TAM )){
    N = atoi(argv[1]);
  }

  int rc, t[N];
  pthread_t threads[N];

	rc = pthread_mutex_init(&mutex, NULL);
	if(rc != 0){
		printf("Fallo en pthread_mutex_init\n");
		exit(-1);
	}

  for(unsigned i = 0 ; i < N ; i++){
		t[i] = i;
    rc = pthread_create(&threads[i], NULL, proceso, (void *)&t[i]);
    if(rc != 0){
      printf("Fallo en pthread_create proceso %d", i);
      exit(-1);
    }
		rc = pthread_cond_init(&c_control[i], NULL);
		if(rc != 0){
			printf("Fallo en pthread_mutex_init\n");
			exit(-1);
		}
  }

	void * status;
	for(unsigned k = 0 ; k < N ; k++){
		rc = pthread_join(threads[k], status);
		if(rc != 0){
			printf("Fallo en pthread_join\n");
			exit(-1);
		}
	}
  return 0;
}
