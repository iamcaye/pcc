#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>

int v_s[3] = {0,0,0};
mqd_t qd;

void * trabajador(void * arg){
  
}

void * sensor(void * arg) {

}

int main (int argc, char *argv[]) {
  int rc, t[3], p;
  pthread_t threads[4];
  void *status;
	mq_attr flags;
	flags.mq_flags = 0;
	flags.mq_maxmsg = 6;
	flags.mq_masize = sizeof(int);
	flags.mq_curmsgs = 0;
	qd = mq_open("micola", O_CREAT, (S_IRUSR|S_IWUSR|S_IRGRUP|S_IROTH), &flags);
  
  rc = pthread_create(&threads[0], NULL, trabajador, (void *)&p);
  if(rc != 0){
    perror("Fallo en pthread_create 0");
    exit(-1);
  }

  for(unsigned b = 0 ; b < 3 ; b++) {
    t[b] = b;
    printf("for %d\n", t[b]);
    rc = pthread_create(&threads[b], NULL, sensor, (void *)&t[b]);
    if(rc != 0){
      perror("Fallo en pthread_create 1");
      exit(-1);
    }
  }

  for (long h = 0 ; h < 4 ; h++) {
    rc = pthread_join(threads[h], &status);
    if (rc != 0) {
      printf("ERROR pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Fin thread %ld estado: %ld\n", h, (long)status);
  }

  return 0;
}
