#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXAF 10
#define MAXDELAY 5

sem_t s_entrada, s_salida;
int n_pers = 0;
pid_t pid;

void * entrada (void * arg) {
  while(1){
    while(n_pers >= MAXAF){
      sem_wait(&s_entrada);
    }

    kill(pid, SIGRTMIN);
    sem_post(&s_salida);
    sleep(1);
  }
}

void * salida (void * arg) {
  while(1){
    sem_wait(&s_salida);
    sleep(rand()%MAXDELAY);
    kill(pid, SIGRTMAX);
    sem_post(&s_entrada);
  }
}

void * gestion (void * arg) {
  int res = -1, info = -1;
  sigset_t gestionset;
  sigemptyset(&gestionset);
  sigaddset(&gestionset, SIGRTMAX);
  sigaddset(&gestionset, SIGRTMIN);
  while(1){
    res = sigwait(&gestionset, &info);
    if(info == SIGRTMIN){
      n_pers++;
      printf("Ha entrado un cliente en el restaurante. Total = %d\n", n_pers);
    }else if(info == SIGRTMAX){
      n_pers--;
      printf("Ha salido un cliente del restaurante. Total = %d\n", n_pers);
    }
  }
}

int main (int argc, char * argv[]) {
  pid = getpid();
  void * status;
  int rc = -1;
  pthread_t h_entrada, h_salida, h_gestion;
  sigset_t mainset;

  sigemptyset(&mainset);
  sigaddset(&mainset, SIGRTMIN);
  sigaddset(&mainset, SIGRTMAX);

  rc = pthread_sigmask(SIG_BLOCK, &mainset, 0);
  if(rc != 0){
    perror("Fallo en pthread_sigmask");
    exit(-1);
  }

  rc = pthread_create(&h_entrada, NULL, entrada, NULL);
  if(rc != 0){
    perror("Fallo en pthread_create");
    exit(-1);
  }

  rc = pthread_create(&h_salida, NULL, salida, NULL);
  if(rc != 0){
    perror("Fallo en pthread_create");
    exit(-1);
  }

  rc = pthread_create(&h_gestion, NULL, gestion, NULL);
  if(rc != 0){
    perror("Fallo en pthread_create");
    exit(-1);
  }

  rc = pthread_join(h_gestion, status);
  if(rc != 0){
    perror("Fallo en pthread_join");
    exit(-1);
  }

  return 0;
}
