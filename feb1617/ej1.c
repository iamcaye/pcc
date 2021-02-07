#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

int nivel = 0;
pid_t pid;

void * gestion (void * arg) {
  sigset_t gestionset;
  int res = -1, info = -1;

  sigemptyset(&gestionset);
  sigaddset(&gestionset, SIGRTMIN);
  sigaddset(&gestionset, SIGRTMAX);
  while(1){
    res = sigwait(&gestionset, &info);
    if(res != 0){
      perror("Fallo en sigwait");
      exit(-1);
    }
    if(info == SIGRTMIN){
      nivel++;
      printf("GESTION: Sube el nivel del tanque. Nivel = %dm.\n", nivel);
    }else if(info == SIGRTMAX && nivel > 0){
      nivel--;
      printf("GESTION: Baja el nivel del tanque. Nivel = %dm.\n", nivel);
    }
  }
}

void * salida (void * arg) {
  while(1){
    sleep(2);
    printf("SALIDA: Envia señal para salida de agua\n");
    kill(pid, SIGRTMAX);
  }
}

void * entrada (void * arg) {
  while(1){
    sleep(1);
    printf("ENTRADA: Envia señal para entrada de agua\n");
    kill(pid, SIGRTMIN);
  }
}

int main (int argc, char * argv[]) {
  int rc = -1;
  void * status;
  pthread_t h_entrada, h_salida, h_gestion;
  sigset_t mainset;

  pid = getpid();

  sigemptyset(&mainset);
  sigaddset(&mainset, SIGRTMIN);
  sigaddset(&mainset, SIGRTMAX);

  rc = pthread_sigmask(SIG_BLOCK, &mainset, NULL);
  if (rc != 0) {
    perror("Fallo en pthread_sigmask");
    exit(-1);
  }

  rc = pthread_create(&h_gestion, NULL, gestion, NULL);
  if (rc != 0) {
    perror("Fallo en pthread_create h_gestion");
    exit(-1);
  }

  rc = pthread_create(&h_entrada, NULL, entrada, NULL);
  if (rc != 0) {
    perror("Fallo en pthread_create h_entrada");
    exit(-1);
  }

  rc = pthread_create(&h_salida, NULL, salida, NULL);
  if (rc != 0) {
    perror("Fallo en pthread_create h_salida");
    exit(-1);
  }

  rc = pthread_join(h_gestion, &status);
  if (rc != 0) {
    perror("Fallo en pthread_join");
    exit(-1);
  }

  return 0;
}
