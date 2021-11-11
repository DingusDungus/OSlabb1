#include <fcntl.h> /* For O_* constants */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char* semName1 = "my_sema1";
const char* semName2 = "my_sema2";

int main(int argc, char** argv) {
  pid_t pid;
  sem_t* sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 1);
  sem_t* sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0);
  int i, status;

  pid = fork();
  if (pid) {
    sem_close(sem_id1);
    sem_close(sem_id2);
    sem_unlink(semName1);
    sem_unlink(semName2);
  } else {
    // child
    sem_close(sem_id1);
    sem_close(sem_id2);
  }
  putchar('\n');
}
