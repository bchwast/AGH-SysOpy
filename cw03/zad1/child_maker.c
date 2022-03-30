//
// Created by bchwast on 24/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char* argv[]) {
    int n;

    if (argc > 1) {
        n = atoi(argv[1]);
    }
    else {
        printf("You need to specify how many child processes you want to create\n");
        exit(1);
    }

    printf("My pid: %d\n", (int) getpid());
    pid_t child_pid;
    for (int i = 0; i < n; i++) {
        child_pid = vfork();
        if (child_pid != 0) {
            continue;
        }
        else {
            printf("Luke, I am %d and he is my father: %d\n",(int) getpid(), (int) getppid());
            exit(0);
        }
    }

    return 0;
}