#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> /* time */
#include <unistd.h>

// Shared variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int chopsticks[5];

struct threadArgs
{
    int id;
};


void printChops()
{
    //Prints chops (used mostly for testing)
    pthread_mutex_lock(&lock);
    for (int i = 0; i < 5; i++)
    {
        printf("%d", chopsticks[i]);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&lock);
}

void initChopsticks()
{
    //Inits chopsticks
    for (int i = 0; i < 5; i++)
    {
        chopsticks[i] = 1;
    }
}

void releaseChopsticks(void *params)
{
    //Releases chopsticks
    struct threadArgs *args = params;
    pthread_mutex_lock(&lock);
    if (args->id == 0)
    {
        chopsticks[0] = 1;
        chopsticks[4] = 1;
    }
    else
    {
        chopsticks[args->id - 1]= 1;
        chopsticks[args->id] = 1;
    }
    pthread_mutex_unlock(&lock);
}

int grabLeft(void *params)
{
    struct threadArgs *args = params;
    int returnInt = 0;
    pthread_mutex_lock(&lock);
    if (args->id == 0)
    {
        if ((chopsticks[4]== 1 && chopsticks[args->id] == 1)) //If condition breaks circular condition causing the deadlock
        {
            chopsticks[4] = 0;
            returnInt = 1;
        }
        else
        {
            returnInt = -1;
        }
    }
    else
    {
        if (chopsticks[args->id - 1] == 1 && chopsticks[args->id] == 1) //If condition breaks circular condition causing the deadlock
        {
            chopsticks[args->id - 1] = 0;
            returnInt = 1;
        }
        else
        {
            returnInt = -1;
        }
    }
    pthread_mutex_unlock(&lock);

    return returnInt;
}

int grabRight(void *params)
{
    struct threadArgs *args = params;
    int returnInt = 0;

    pthread_mutex_lock(&lock);
    if (chopsticks[args->id] == 1) //Right function works exactly the same
    {
        chopsticks[args->id] = 0;
        returnInt = 1;
    }
    else
    {
        returnInt = -1;
    }
    pthread_mutex_unlock(&lock);

    return returnInt;
}

void *child(void *params)
{
    time_t t;
    srand((unsigned)time(&t));
    struct threadArgs *args = (struct threadArgs *)params;
    int loopAlways = 1;
    int leftGrabbed = 0;
    int rightGrabbed = 0;


    while (loopAlways)
    {
        printf("Professor %d joined the table, waiting...\n", args->id);
        int randomSleep = (rand() % 8) + 2; //Gives initial sleep random time between 2 and 10 seconds
        sleep(randomSleep);
        fflush(stdout);

        while (leftGrabbed == 0)
        {
            sleep(5);
            if (grabLeft(args) == 1) //Calls on grabLeft, if it succeeds go on
            {
                printf("Professor %d grabbed left chopstick, waiting...\n", args->id);
                fflush(stdout);
                leftGrabbed = 1; //Sets left fork to grabbed
                randomSleep = (rand() % 2) + 1; //Random time between 1 - 3 seconds
                sleep(randomSleep);
            }
            else //If it fails 
            {
                printf("Professor %d tried to grab its left chopstick\n", args->id); //If it fails to grab chopstick
                fflush(stdout);
            }
        }
        while (rightGrabbed == 0) //Calls on grabRight, if it succeeds go on
        {
            sleep(5);
            if (grabRight(args) == 1)
            {
                printf("Thread %d grabbed right chopstick, eating...\n", args->id);
                fflush(stdout);
                rightGrabbed = 1; //Sets right fork to grabbed
                randomSleep = (rand() % 10) + 10; //Random wait from 10 - 20 seconds
                sleep(randomSleep);
            }
            else //If it fails 
            {
                printf("Thread %d tried to grab its right chopstick\n", args->id); //If it fails to grab chopstick
                fflush(stdout);
            }
        }
        printf("Thread %d lays down its chopsticks on the table\n", args->id); //Has eaten and lays down its forks
        fflush(stdout);
        leftGrabbed = 0;
        rightGrabbed = 0;
        releaseChopsticks(args);
    }
    free(args);

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t *children;
    unsigned int id = 0;
    unsigned int nThreads = 5;

    initChopsticks(); //Inits chopsticks before other threads start working on it

    struct threadArgs *args;

    if (argc > 1)
        nThreads = atoi(argv[1]);
    children = malloc(nThreads * sizeof(pthread_t));
    for (id = 0; id < nThreads; id++)
    {
        args = malloc(sizeof(struct threadArgs)); //Allocates args for thread
        args->id = id;
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
