#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> /* time */
#include <unistd.h>

int chopsticks[5]; //Shared chopsticks for the table
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //Lock to hinder two professors reading or grabbing chopsticks on the same time

struct threadArgs
{
    int id;
};

void initChopsticks()
{
    //Initializes and sets all chopsticks to free i.e., 1
    for (int i = 0; i < 5; i++)
    {
        chopsticks[i] = 1;
    }
}


int grabLeft(unsigned long id)
{
    pthread_mutex_lock(&lock);
    if (id == 0)
    {
        if (chopsticks[4] == 1)
        {
            chopsticks[4] = 0; //Sets chopstick to grabbed
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
        if (chopsticks[id - 1] == 1)
        {
            chopsticks[id - 1] = 0; //Sets chopstick to grabbed
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

int grabRight(unsigned long id)
{
    pthread_mutex_lock(&lock);
    if (chopsticks[id] == 1)
    {
        chopsticks[id] = 0; //Sets chopstick to grabbed
        pthread_mutex_unlock(&lock);
        return 1;
    }
    else
    {
        pthread_mutex_unlock(&lock);
        return -1;
    }
}

void releaseChopsticks(unsigned long id)
{
    //Releases chopsticks back onto table
    pthread_mutex_lock(&lock);
    if (id == 0)
    {
        chopsticks[4] = 1; //Sets chopstick to free
        chopsticks[id] = 1; //Sets chopstick to free
    }
    else
    {
        chopsticks[id - 1] = 1; //Sets chopstick to free
        chopsticks[id] = 1; //Sets chopstick to free
    }
    pthread_mutex_unlock(&lock);
}

void *child(void *params)
{
    time_t t;
    srand((unsigned)time(&t));
    struct threadArgs *args = (struct threadArgs *)params;
    printf("Professor %d joined the table, waiting...\n", args->id);
    int randomSleep = (rand() % 8) + 2; //Gives initial sleep random time between 2 and 10 seconds
    sleep(randomSleep);

    int leftGrabbed = 0;
    int rightGrabbed = 0;

    while (leftGrabbed == 0)
    {
        sleep(5);
        if (grabLeft(args->id) == 1)
        {
            printf("Professor %d grabbed left chopstick, waiting...\n", args->id);
            fflush(stdout);
            leftGrabbed = 1; //Sets left fork to grabbed
            randomSleep = (rand() % 2) + 1; //Random time between 1 - 3 seconds
            sleep(randomSleep);
        }
        else
        {
            printf("Professor %d tried to grab its left chopstick\n", args->id); //If it fails to grab chopstick
            fflush(stdout);
        }
    }
    while (rightGrabbed == 0)
    {
        sleep(5);
        if (grabRight(args->id) == 1)
        {
            printf("Professor %d grabbed right chopstick, eating...\n", args->id);
            fflush(stdout);
            rightGrabbed = 1; //Sets right fork to grabbed
            randomSleep = (rand() % 10) + 10; //Random wait from 10 - 20 seconds
            sleep(randomSleep);
        }
        else
        {
            printf("Professor %d tried to grab its right chopstick\n", args->id); //If it fails to grab chopstick
            fflush(stdout);
        }
    }
    printf("Professor %d lays down its chopsticks on the table\n", args->id); //Has eaten and lays down its forks
    releaseChopsticks(args->id);
    fflush(stdout);

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
    sleep(100);
    for (id = 0; id < nThreads; id++)
        pthread_join(children[id - 1], NULL);
    printf("\nAll %d have eaten.\n\n", nThreads);
    free(children);
    pthread_mutex_destroy(&lock);
    return 0;
}
