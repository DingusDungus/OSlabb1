/***************************************************************************
 *
 * Parallel version of Matrix-Matrix multiplication
 * task 17, parallel init.
 *
 ***************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define NR_OF_THREADS 256

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];
pthread_barrier_t barrier;

struct threadArgs
{
    int start;
    int end;
};

static void
init_matrix(int start, int end)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = start; j < end; j++)
        {
            /* Simple initialization, which enables us to easy check
	         * the correct answer. Each element in c will have the same
	         * value as SIZE after the matmul operation.
	         */
            a[i][j] = 1.0;
            b[i][j] = 1.0;
        }
}

static void
matmul_seq()
{
    int i, j, k;

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
            printf(" %7.2f", c[i][j]);
        printf("\n");
    }
    fflush(stdout);
}

void mulRow(int start, int end)
{
    int i, j, k;

    for (i = 0; i < SIZE; i++)
    {
        for (j = start; j < end; j++)
        {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

void *child(void *params)
{
    struct threadArgs *args = (struct threadArgs *)params;
    int start = args->start;
    int end = args->end;
    init_matrix(start, end);
    pthread_barrier_wait(&barrier);
    mulRow(start, end);
    free(args);
    return NULL;
}


int main(int argc, char **argv)
{
    struct threadArgs *args;

    pthread_t *children;
    int id = 0;
    pthread_barrier_init(&barrier,NULL,NR_OF_THREADS);

    int work = SIZE/NR_OF_THREADS;
    children = malloc(NR_OF_THREADS * sizeof(pthread_t));
    for (id = 0; id < NR_OF_THREADS; id++)
    {
        args = malloc(sizeof(struct threadArgs));
        args->start = work * (id);
        args->end = work * (id+1);
        pthread_create(&(children[id]), NULL, child, (void *)args);
    }
    for (id = 0; id < NR_OF_THREADS; id++)
    {
        pthread_join(children[id], NULL);
    }
    free(children);
}
