#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> /* time */
#include <unistd.h>


pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void printChops(int *chopsticks)
{
    for (int i = 0;i < 5;i++)
    {
        printf("%d", chopsticks[i]);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

struct threadArgs {
    int id;
    int *chopsticks;
    int leftGrabbed;
    int rightGrabbed;
};

void initChopsticks(int *chopsticks)
{
    for (int i = 0;i < 5;i++)
    {
        chopsticks[i] = 1;
    }

}

// Shared Variables

int grabLeft(unsigned long id, int *chopsticks) 
{
    if (id == 0)
    {
        if (chopsticks[4] == 1)
        {
            chopsticks[4] = 0;
            return 1;
        }
        else 
        {
            return -1;
        }
    }
    else
    {
        if (chopsticks[id - 1] == 1)
        {
            chopsticks[id - 1] = 0;
            return 1;
        }
        else 
        {
            return -1;
        }
    }
}

int grabRight(unsigned long id, int *chopsticks) 
{
    if (id == 4)
    {
        if (chopsticks[0] == 1)
        {
            chopsticks[0] = 0;
            return 1;
        }
        else 
        {
            return -1;
        }
    }
    else
    {
        if (chopsticks[id + 1] == 1)
        {
            chopsticks[id + 1] = 0;
            return 1;
        }
        else 
        {
            return -1;
        }
    }
}

void releaseChopsticks(unsigned long id, int *chopsticks)
{
    if (id == 0)
    {
        chopsticks[4] = 1;
        chopsticks[1] = 1;
    }
    else if (id == 4)
    {
        chopsticks[3] = 1;
        chopsticks[0] = 1;
    }
    else
    {
        chopsticks[id - 1] = 1;
        chopsticks[id + 1] = 1;
    } 
}

void* child(void* params) {
    time_t t;
    srand((unsigned)time(&t));
    struct threadArgs *args = (struct threadArgs*) params;
    printf("Thread %d joined the table, waiting...\n", args->id);
    int randomSleep = (rand() % 8) + 2;
    sleep(randomSleep);
    fflush(stdout);

    while (args->leftGrabbed == 0)
    {
        sleep(5);
        if (grabLeft(args->id, args->chopsticks) == 1)
        {
            printf("Thread %d grabbed left chopstick, waiting...\n", args->id);
            fflush(stdout);
            args->leftGrabbed = 1;
            printChops(args->chopsticks);
            randomSleep = (rand() % 3) + 1;
            sleep(randomSleep);
        }
        else
        {
            printf("Thread %d tried to grab its left chopstick\n", args->id);
            fflush(stdout);
        }
    }
    while (args->rightGrabbed == 0)
    {
        sleep(5);
        if (grabRight(args->id, args->chopsticks) == 1)
        {
            printf("Thread %d grabbed right chopstick, eating...\n", args->id);
            fflush(stdout);
            args->rightGrabbed = 1;
            printChops(args->chopsticks);
            randomSleep = (rand() % 10) + 10;
            sleep(randomSleep);
        }
        else
        {
            printf("Thread %d tried to grab its right chopstick\n", args->id);
            fflush(stdout);
        }
    }
    printf("Thread %d lays down its chopsticks on the table\n", args->id);
    fflush(stdout);

    free(args);

    return NULL;
}

int main(int argc, char** argv) {
    pthread_t *children;
    unsigned int id = 0;
    unsigned int nThreads = 5;

    int chopsticks[5];
    initChopsticks(chopsticks);

    struct threadArgs* args;

    if (argc > 1)
        nThreads = atoi(argv[1]);
    children = malloc( nThreads * sizeof(pthread_t) );
    for (id = 0; id < nThreads; id++) {
        args = malloc(sizeof(struct threadArgs));
		args->id = id;
		args->chopsticks = chopsticks;
        args->leftGrabbed = 0;
        args->rightGrabbed = 0;
        fflush(stdout);
        pthread_create(&(children[id]), NULL, child, (void*)args);
    }
    for (id = 0; id < nThreads; id++) {
        pthread_join(children[id], NULL);
    }
    printf("\nAll %d have eaten.\n\n", nThreads);
    free(children);
    return 0;
}
