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
    const int BUFFER_SIZE = 10;  // Size of shared buffer
    struct shm_struct {
        int buffer[BUFFER_SIZE];  // Shared buffer
        unsigned empty;           // Variable for if buffer is full or not
        unsigned sent;            // Variable for producer sent
        unsigned recieved;        // Variable for consumer recieved
    };
    unsigned index;     // Local index variable
    float randomSleep;  // Float variable for the sleeping length
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
    randomSleep = 0;
    shmp->empty = 1;
    shmp->sent = 0;
    pid = fork();  // Creates child process

    if (pid != 0) {
        /* here's the parent, acting as producer */
        while (var1 < 100) {
            if (index > BUFFER_SIZE - 1) {
                index = 0;  // Circular iteration through buffer
            }
            if ((shmp->sent - shmp->recieved) >= BUFFER_SIZE) {
                shmp->empty = 0;  // If producer sends too much it is stopped
                                  // through active wait
            }

            /* write to shmem */
            var1++;
            while (shmp->empty == 0)
                ;
            shmp->buffer[index] = var1;  // Writes to given position
            printf("Sent %d to index %d\n", var1, index);
            shmp->sent++;                // Increases sent amount
            index++;  // Increments index so producer writes on next position in
                      // array in next loop

            randomSleep = rand() % 20 + 1;  // Creates random sleep value
            sleep((randomSleep / 10));      // Sleeps for random time
        }
        /*Detaching from shared memory */
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    } else {
        /* here's the child, acting as consumer */
        while (var2 < 100) {
            while ((shmp->sent - shmp->recieved) <= 0)
                ;
            /* read from shmem */
            var2 = shmp->buffer[index];  // Reads on given position
            printf("Received %d from index %d\n", var2, index);
            shmp->recieved++;  // increases read amount
            index++;  // Increments index so consumer reads on next position in
                      // array in next loop

            if (index > BUFFER_SIZE - 1) {
                index = 0;  // Circular iteration through buffer
            }
            if ((shmp->sent - shmp->recieved) >= BUFFER_SIZE) {
                shmp->empty = 0;  // If producer sends too much it is stopped
                                  // through active wait
            } else {
                shmp->empty = 1;  // If consumer is up to speed with producer
                                  // again, wake up producer
            }

            randomSleep = rand() % 20 + 1;
            sleep((randomSleep / 10));
        }
        /*Detaching from shared memory */
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    }
    return 0;
}
