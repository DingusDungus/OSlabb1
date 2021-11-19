#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Shared Variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //Inits mutex which will be important to sync threads
double bankAccountBalance = 0;

void deposit(double amount) {
    bankAccountBalance += amount; //Deposits money into shared account
}

void withdraw(double amount) {
    bankAccountBalance -= amount; //Withdraws money from shared account
}

// utility function to identify even-odd numbers
unsigned odd(unsigned long num) {
    return num % 2; //Return either 1 or 0, odd or even
}

// simulate id performing 1000 transactions
void do1000Transactions(unsigned long id) {
    pthread_mutex_lock(&lock); //Locks so only current thread can enter this critical region
    for (int i = 0; i < 1000; i++) {
        if (odd(id))
            deposit(100.00); // odd threads deposit
        else
            withdraw(100.00); // even threads withdraw
    }
    pthread_mutex_unlock(&lock); //Unlocks it so another thread can do it afterwards
}

void* child(void* buf) {
    unsigned long childID = (unsigned long)buf;
    do1000Transactions(childID); //Function which does 1000 transactions
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t *children;
    unsigned long id = 0;
    unsigned long nThreads = 0;
    if (argc > 1)
        nThreads = atoi(argv[1]); //Takes in an argv
    children = malloc( nThreads * sizeof(pthread_t) );
    for (id = 1; id < nThreads; id++)
        pthread_create(&(children[id-1]), NULL, child, (void*)id); //Creates children
    do1000Transactions(0); // main thread work (id=0)
    for (id = 1; id < nThreads; id++)
        pthread_join(children[id-1], NULL);
    printf("\nThe final account balance with %lu threads is $%.2f.\n\n", nThreads, bankAccountBalance);
    free(children);
    pthread_mutex_destroy(&lock); //Destroys mutex lock
    return 0;
}
