#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    pid_t pid;
    pid_t pid1;

    unsigned i;
    unsigned niterations = 1000;
    pid = fork();
    if (pid == 0) {
        for (i = 0; i < niterations; ++i)
            printf("A = %d id = %d pid = %d, ", i, pid, getpid());

        printf("\n");
    } else {
        pid1 = fork();
        if (pid1 == 0) {
            for (i = 0; i < niterations; ++i)
                printf("B = %d id = %d pid = %d, ", i, pid1, getpid());

            printf("\n");
        } else {
            for (i = 0; i < niterations; ++i)
                printf("C = %d pid = %d, ", i, getpid());

            printf("\n");
            printf(
                "Parent process id = %d \n1st child process id = %d \n2nd "
                "child process id = %d \n",
                getpid(), pid, pid1);
        }
    }

    printf("\n");
    // return 0;
}
