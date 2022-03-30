//
// Created by bchwast on 30/03/2022.
//

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


void check_signal_pending() {
    sigset_t pending_set;
    sigpending(&pending_set);

    sigismember(&pending_set, SIGUSR1) ? printf("Signal %d is pending\n", SIGUSR1) : printf("Signal %d is not pending\n", SIGUSR1);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Something went wrong with executing sub-program\n");
        exit(1);
    }

    if (strcmp(argv[1], "pending") != 0) {
        printf("Raising a signal in exec child...\n");
        raise(SIGUSR1);
    }
    printf("Child exec: ");
    check_signal_pending();

    return 0;
}