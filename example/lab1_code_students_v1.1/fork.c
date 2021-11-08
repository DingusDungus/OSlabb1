#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid;
    unsigned i;
    unsigned niterations = 1000;
    pid = fork();
    if (pid == 0) {
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
     } else {
        for (i = 0; i < niterations; ++i)
            printf("B = %d, ", i);
    }
    printf("\n");
}
