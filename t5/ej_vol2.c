#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void *hebra(void * arg){
  int signo = *(int *)arg, info;
  sigset_t sigset;

  sigemptyset(&sigset);
  sigaddset(&sigset, signo);

  while(1){
    printf("Hebra esperando\n");
    signo = sigwait(&sigset, &info);
    if(signo != -1){
      if(info == SIGINT){
        printf("Heeeeello SIGINT\n");
      }else if(info == SIGUSR1){
        printf("Heeeeello SIGUSR1\n");
      }
    }
  }
}

int main(int argc, void *argv[]){
  struct sigaction act;
  sigset_t sigset;
  int rc = 0;
  pthread_t threads[2];
  void * status;

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigaddset(&sigset, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL);
  int sigint = 2, sigusr1 = 10;

  rc = pthread_create(&threads[0], NULL, hebra, (void *)&sigint);
  if(rc != 0){
    perror("Fallo en pthread_create 0");
    exit(-1);
  }

  rc = pthread_create(&threads[1], NULL, hebra, (void *)&sigusr1);
  if(rc != 0){
    perror("Fallo en pthread_create 1");
    exit(-1);
  }

  for(unsigned i = 0; i < 2 ; i++){
    rc = pthread_join(threads[i], status);
    if(rc != 0){
      perror("Fallo en pthread_join");
      exit(-1);
    }
  }
  return 0;
}
