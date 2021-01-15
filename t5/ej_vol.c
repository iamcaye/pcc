#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void manejador1(int signo, siginfo_t *info, void *context){
  if(signo == SIGINT){
    printf("Heeeeello SIGINT\n");
  }else if(signo == SIGUSR1){
    printf("Heeeeello SIGUSR1\n");
  }
}

int main(int argc, void *argv[]){
  struct sigaction act;
  sigset_t sigset;

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigaddset(&sigset, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL);

  act.sa_sigaction = manejador1;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = SA_SIGINFO;

  if(sigaction(SIGINT, &act, NULL) < 0){
    perror("sigaction ha fallado");
  }

  if(sigaction(SIGUSR1, &act, NULL) < 0){
    perror("sigaction ha fallado");
  }

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigaddset(&sigset, SIGUSR1);
  pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);


  while(1){
  
  }
  return 0;
  
}
