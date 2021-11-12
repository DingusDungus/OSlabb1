#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>  /* time */
#include <fcntl.h> /* For O_* constants */
#include <semaphore.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct threadArgs
{
    int id;
    struct tableContent *chopsticks;
    int leftGrabbed;
    int rightGrabbed;
};

struct tableContent
{
    int locked;
    int stickGrabbed;
};

void printChops(struct tableContent *chopsticks)
{
    for (int i = 0; i < 5; i++)
    {
        printf("%d", chopsticks[i].stickGrabbed);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

void initChopsticks(struct tableContent *chopsticks)
{
    for (int i = 0; i < 5; i++)
    {
        chopsticks[i].stickGrabbed = 1;
        chopsticks[i].locked = 0;
    }
}

// Shared Variables

void releaseChopsticks(void *params)
{
    struct threadArgs *args = params;
    pthread_mutex_lock(&lock);
    if (args->id == 0)
    {
        args->chopsticks[4].stickGrabbed = 1;
        args->chopsticks[1].stickGrabbed = 1;
        args->chopsticks[args->id + 1].locked = 0;
        args->chopsticks[4].locked = 0;
    }
    else if (args->id == 4)
    {
        args->chopsticks[3].stickGrabbed = 1;
        args->chopsticks[0].stickGrabbed = 1;
        args->chopsticks[0].locked = 0;
        args->chopsticks[args->id - 1].locked = 0;
    }
    else
    {
        args->chopsticks[args->id - 1].stickGrabbed = 1;
        args->chopsticks[args->id + 1].stickGrabbed = 1;
        args->chopsticks[args->id - 1].locked = 0;
        args->chopsticks[args->id + 1].locked = 0;
    }
    pthread_mutex_unlock(&lock);
}

int grabLeft(void *params)
{
    struct threadArgs *args = params;
    if (args->id == 0)
    {
        pthread_mutex_lock(&lock);
        if (args->chopsticks[4].stickGrabbed == 1 && args->chopsticks[args->id + 1].locked == 0)
        {
            args->chopsticks[4].stickGrabbed = 0;
            args->chopsticks[args->id + 1].locked = 1;
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
    else if (args->id == 4)
    {
        pthread_mutex_lock(&lock);
        if (args->chopsticks[args->id - 1].stickGrabbed == 1 && args->chopsticks[0].locked == 0)
        {
            args->chopsticks[args->id - 1].stickGrabbed = 0;
            args->chopsticks[0].locked = 1;
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
    else
    {
        pthread_mutex_lock(&lock);
        if (args->chopsticks[args->id - 1].stickGrabbed == 1 && args->chopsticks[args->id + 1].locked == 0)
        {
            args->chopsticks[args->id - 1].stickGrabbed = 0;
            args->chopsticks[args->id + 1].locked = 1;
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
}

int grabRight(void *params)
{
    struct threadArgs *args = params;
    if (args->id == 0)
    {
        pthread_mutex_lock(&lock);
        if (args->chopsticks[args->id + 1].stickGrabbed == 1 && args->chopsticks[4].locked == 0)
        {
            args->chopsticks[args->id + 1].stickGrabbed = 0;
            args->chopsticks[4].locked = 1;
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
    else if (args->id == 4)
    {
        pthread_mutex_lock(&lock);
        if (args->chopsticks[0].stickGrabbed == 1 && args->chopsticks[args->id - 1].locked == 0)
        {
            args->chopsticks[0].stickGrabbed = 0;
            args->chopsticks[args->id - 1].locked = 1;
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
    else
    {
        pthread_mutex_lock(&lock);
        if (args->chopsticks[args->id + 1].stickGrabbed == 1 && args->chopsticks[args->id - 1].locked == 0)
        {
            args->chopsticks[args->id + 1].stickGrabbed = 0;
            args->chopsticks[args->id - 1].locked = 1;
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
}

void *child(void *params)
{
    time_t t;
    srand((unsigned)time(&t));
    struct threadArgs *args = (struct threadArgs *)params;
    printf("Thread %d joined the table, waiting...\n", args->id);
    int randomSleep = (rand() % 8) + 2;
    sleep(randomSleep);
    fflush(stdout);

    while (args->leftGrabbed == 0)
    {
        sleep(5);
        if (grabLeft(args) == 1)
        {
            printf("Thread %d grabbed left chopstick, waiting...\n", args->id);
            fflush(stdout);
            args->leftGrabbed = 1;
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
        if (grabRight(args) == 1)
        {
            printf("Thread %d grabbed right chopstick, eating...\n", args->id);
            fflush(stdout);
            args->rightGrabbed = 1;
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

int main(int argc, char **argv)
{
    pthread_t *children;
    unsigned int id = 0;
    unsigned int nThreads = 5;

    struct tableContent chopsticks[5];
    initChopsticks(chopsticks);

    struct threadArgs *args;

    if (argc > 1)
        nThreads = atoi(argv[1]);
    children = malloc(nThreads * sizeof(pthread_t));
    for (id = 0; id < nThreads; id++)
    {
        args = malloc(sizeof(struct threadArgs));
        args->id = id;
        args->chopsticks = chopsticks;
        args->leftGrabbed = 0;
        args->rightGrabbed = 0;
        fflush(stdout);
        pthread_create(&(children[id]), NULL, child, (void *)args);
    }
    for (id = 0; id < nThreads; id++)
    {
        pthread_join(children[id], NULL);
    }
    printf("\nAll %d have eaten.\n\n", nThreads);
    free(children);
    pthread_mutex_destroy(&lock);
    return 0;
}
