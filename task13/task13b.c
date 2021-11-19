#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> /* time */
#include <unistd.h>

int chopsticks[5]; //Shared chopsticks for the table
int professorTurn = 1; //Shared integer which decides order of professors
int cycleActive = 1;
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

int chopstickScheduler(int id) //Scheduling function which decides the order professors eat in so they don't deadlock
{
    if (id == professorTurn && cycleActive == 1)
    {
        if (id == 0 && (chopsticks[id] == 1 && chopsticks[4] == 1))
        {
            professorTurn += 2;
        }
        else if (chopsticks[id] == 1 && chopsticks[id - 1] == 1)
        {
            if (professorTurn == 5) { professorTurn = 0; } //Circular so it restarts at index 0 (i.e., 1->3->0)
            else if (professorTurn == 6) { professorTurn = 1; } //Circular so it restarts at index 1 (i.e., 2->4->1)
            professorTurn += 2;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int grabLeft(unsigned long id)
{
    pthread_mutex_lock(&lock);
    if (id == 0 && chopstickScheduler(id) == 1) //Calls on the scheduler to 
    {
        if (chopsticks[4] == 1)
        {
            chopsticks[4] = 0; //Sets chopstick to grabbed
            pthread_mutex_unlock(&lock);
            return 1;
        }
        {
            pthread_mutex_unlock(&lock);
            return 0;
        }
    }
    else
    {
        if (chopsticks[id - 1] == 1 && chopstickScheduler(id) == 1)
        {
            chopsticks[id - 1] = 0; //Sets chopstick to grabbed
            pthread_mutex_unlock(&lock);
            return 1;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            return 0;
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
        return 0;
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

