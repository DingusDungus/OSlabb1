#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct threadArgs
{
    int id;
    /*int leftGrabbed;*/
    /*int rightGrabbed;*/
};

// Shared Variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// philosopher/thread logic
void logic(unsigned long id) {
    unsigned int leftGrabbed = 0; // set to 0, no chopstick.
    unsigned int rightGrabbed = 0; // set to 0, no chopstick.
    unsigned int hasEaten = 0; // set to 0, has not eaten.
    int pos = id+1;
    pthread_mutex_lock(&lock);
    printf("Im philosopher %d\n", pos);
    while (hasEaten == 0) {

    }
    pthread_mutex_unlock(&lock);
}

void* child(void* buf) {
    unsigned long id = (unsigned long)buf;
    logic(id);
    printf("blablablabl\n");
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t *children;
    unsigned long id = 1;
    unsigned long nThreads = 5;

    int chopsticks[5];
    // initialize array with chopsticks.
    // chopstick = 1, no chopstick = 0
    for (int i = 0; i < 5; i++) {
      chopsticks[i] = 1;
      printf("testtest \n");
    }
    struct threadArgs* args;

    children = malloc( nThreads * sizeof(pthread_t) );
    for (id = 1; id < nThreads; id++)
        pthread_create(&(children[id-1]), NULL, child, (void*)id);
    logic(0); // main thread work (id=1)
      printf("after logic \n");
    for (id = 1; id < nThreads; id++)
        pthread_join(children[id-1], NULL);
    printf("\nNumber of threads is %lu", nThreads);
    free(children);
    pthread_mutex_destroy(&lock);
    return 0;
}
