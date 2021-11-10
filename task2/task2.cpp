#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h> /* time */
#include <unistd.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

int main(int argc, char** argv) {
    const int BUFFER_SIZE = 10;
    struct shm_struct {
        int buffer[BUFFER_SIZE];
        unsigned empty;
        unsigned sent;
    };
    unsigned index;
    float randomSleep = 0;
    volatile struct shm_struct* shmp = NULL;
    char* addr = NULL;
    pid_t pid = -1;
    int var1 = 0, var2 = 0, shmid = -1;
    struct shmid_ds* shm_buf;
    time_t t;
    srand((unsigned)time(&t));

    /* allocate a chunk of shared memory */
    shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
    shmp = (struct shm_struct*)shmat(shmid, addr, 0);
    index = 0;
    shmp->empty = 1;
    shmp->sent = 0;
    pid = fork();

    if (pid != 0) {
        /* here's the parent, acting as producer */
        while (var1 < 100) {
            if (index > BUFFER_SIZE - 1) {
                index = 0;
            }

            /* write to shmem */
            var1++;
            while (shmp->empty == 0)
                ;
            printf("Sending %d to index %d\n", var1, index);
            fflush(stdout);
            shmp->buffer[index] = var1;
            shmp->sent++;
            index++;

            randomSleep = rand() % 20 + 1;
            sleep((randomSleep / 10));
        }
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    } else {
        /* here's the child, acting as consumer */
        unsigned recieved = 0;
        while (var2 < 100) {
            while ((shmp->sent - recieved) <= 0)
                ;
            /* read from shmem */
            var2 = shmp->buffer[index];
            printf("Received %d from index %d\n", var2, index);
            fflush(stdout);
            recieved++;
            index++;

            if (index > BUFFER_SIZE - 1) {
                index = 0;
            }
            if (shmp->empty == 0 && (shmp->sent - recieved) > BUFFER_SIZE) {
                index = 0;
                shmp->empty = 0;
            } else {
                shmp->empty = 1;
            }

            randomSleep = rand() % 20 + 1;
            sleep((randomSleep / 10));
        }
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    }
    return 0;
}
