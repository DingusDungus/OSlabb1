#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h> 
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> /* For O_* constants */

#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";

int main(int argc, char **argv)
{
	struct shm_struct {
		int buffer[10];
	};
    unsigned index;
    float randomSleep = 0;
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;
    time_t t;
    srand((unsigned) time(&t));

    sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 1);
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0);

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
    index = 0;
	pid = fork();
    
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
            if (index > 9)
            {
                index = 0;
            }
            
            sem_wait(sem_id1);
			/* write to shmem */
			var1++;
            printf("Sending %d from index %d\n", var1, index); fflush(stdout);
			shmp->buffer[index] = var1;
            index++;
            sem_post(sem_id2);

            randomSleep = rand() % 20 + 1;
            sleep((randomSleep/10));
		}

        sem_close(sem_id1);
	    sem_close(sem_id2);
	    sem_unlink(semName1);
	    sem_unlink(semName2);

		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
            if (index > 9)
            {
                index = 0;
            }

            sem_wait(sem_id2);
			/* read from shmem */
			var2 = shmp->buffer[index];
			printf("Received %d from index %d\n", var2, index); fflush(stdout);
            index++;
            sem_post(sem_id1);

            randomSleep = rand() % 20 + 1;
            sleep((randomSleep/10));
		}

        sem_close(sem_id1);
	    sem_close(sem_id2);

		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
    return 0;
}