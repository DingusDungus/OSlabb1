#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* time */
#include <unistd.h>

// Shared Variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int chopsticks[5];

void printChopsticks() {
    // chopstick = 1, no chopstick = 0
    for (int i = 0; i < 5; i++) {
        printf("chopstick %d, value %d \n", i, chopsticks[i]);
        fflush(stdout);
    }
}

void setChopstickPos(unsigned int pos, unsigned int* leftStick,
                     unsigned int* rightStick) {
    if (pos == 4) {
        *rightStick = 0;
    } else {
        *rightStick = pos + 1;
    }
    if (pos == 0) {
        *leftStick = 4;
    } else {
        *leftStick = pos - 1;
    }
}

void think1To3(unsigned int pos) {
    int randomSleep = (rand() % 2) + 1;
    printf("Im philosopher %d, ", pos);
    printf("Thinking for %ds...\n", randomSleep);
    fflush(stdout);
    sleep(randomSleep);
}

void think2To8(unsigned int pos) {
    int randomSleep = (rand() % 8) + 2;
    printf("Im philosopher %d, ", pos);
    printf("Thinking for %ds...\n", randomSleep);
    fflush(stdout);
    sleep(randomSleep);
}

void eat(unsigned int pos) {
    int randomSleep = (rand() % 10) + 10;
    printf("Im philosopher %d, ", pos);
    printf("Eating for %ds...\n", randomSleep);
    fflush(stdout);
    sleep(randomSleep);
}

// philosopher/thread logic
void logic(unsigned long id) {
    unsigned int leftGrabbed = 0;   // set to 0, no chopstick.
    unsigned int rightGrabbed = 0;  // set to 0, no chopstick.
    unsigned int hasEaten = 0;      // set to 0, has not eaten.
    unsigned int pos = id;
    unsigned int leftStickPos;
    unsigned int rightStickPos;
    setChopstickPos(pos, &leftStickPos, &rightStickPos);
    printf("Im philosopher %d, ", pos);
    printf("My left stick pos is %d My right stick pos is %d\n", leftStickPos,
           rightStickPos);
    fflush(stdout);

    while (hasEaten == 0) {
        // think
        fflush(stdout);
        think2To8(pos);
        // grab left
        if (leftGrabbed == 0) {
            pthread_mutex_lock(&lock);
            printf("Ph %d Trying to grab left chopstick...\n", pos);
            fflush(stdout);
            if (chopsticks[leftStickPos] == 1) {  // chopstick is 1, pickable.
                chopsticks[leftStickPos] = 0;     // pickup chopstick.
                leftGrabbed = 1;                  // picked up chopstick.
                printf("Ph %d grabbed left chopstick...\n", pos);
                fflush(stdout);
            } else {
                printf("Ph %d could NOT grab left chopstick...\n", pos);
                fflush(stdout);
            }
            pthread_mutex_unlock(&lock);
            printChopsticks();
        } else {
            printf("Ph %d Already has left chopstick...\n", pos);
            fflush(stdout);
        }
        printf("Ph %d left stick: %d rightstick: %d\n", pos, leftGrabbed,
               rightGrabbed);
        fflush(stdout);
        // grab right
        if (rightGrabbed == 0 && leftGrabbed == 1) {
            pthread_mutex_lock(&lock);
            printf("Ph %d Trying to grab right chopstick...\n", pos);
            fflush(stdout);
            if (chopsticks[rightStickPos] == 1) {  // chopstick is 1, pickable.
                chopsticks[rightStickPos] = 0;     // pickup chopstick.
                rightGrabbed = 1;                  // picked up chopstick.
                printf("Ph %d grabbed right chopstick...\n", pos);
                fflush(stdout);
            } else {
                printf("Ph %d could NOT grab right chopstick...\n", pos);
                fflush(stdout);
            }
            pthread_mutex_unlock(&lock);
            printChopsticks();
        } else if (rightGrabbed == 1) {
            printf("Ph %d Already has right chopstick...\n", pos);
            fflush(stdout);
        } else {
            printf("Ph %d Has to pick up left before right...\n", pos);
            fflush(stdout);
        }
        printf("Ph %d left stick: %d rightstick: %d\n", pos, leftGrabbed,
               rightGrabbed);
        fflush(stdout);
        if (leftGrabbed == 1 && rightGrabbed == 1) {
            eat(pos);  // sleeps between 10-20 seconds.
            pthread_mutex_lock(&lock);
            chopsticks[leftStickPos] = 1;   // Put down left sick
            chopsticks[rightStickPos] = 1;  // Put down right sick
            pthread_mutex_unlock(&lock);
        }
    }
}

void* child(void* buf) {
    unsigned long id = (unsigned long)buf;
    printf("Child %lu starting...\n", id);
    fflush(stdout);
    logic(id);
    printf("Child %lu is done\n", id);
    fflush(stdout);
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t* children;
    unsigned long id = 1;
    unsigned long nThreads = 5;

    // initialize array with chopsticks.
    // chopstick = 1, no chopstick = 0
    for (int i = 0; i < 5; i++) {
        chopsticks[i] = 1;
        printf("chopstick %d, value %d \n", i, chopsticks[i]);
        fflush(stdout);
    }

    children = malloc(nThreads * sizeof(pthread_t));
    for (id = 1; id < nThreads; id++)
        pthread_create(&(children[id - 1]), NULL, child, (void*)id);
    logic(0);  // main thread work (id=1)
    for (id = 1; id < nThreads; id++)
        pthread_join(children[id - 1], NULL);
    printf("\nNumber of threads is %lu", nThreads);
    fflush(stdout);
    free(children);
    pthread_mutex_destroy(&lock);
    return 0;
}
