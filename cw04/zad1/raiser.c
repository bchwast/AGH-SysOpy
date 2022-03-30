//
// Created by bchwast on 30/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef FORK
    #include <sys/wait.h>


    void handler(int sig_num) {
        printf("Hello there, I have received this signal: %d, my PID: %d, my PPID: %d\n", sig_num, (int) getpid(), (int) getppid());
    }
#endif


void check_signal_pending() {
    sigset_t pending_set;
    sigpending(&pending_set);

    sigismember(&pending_set, SIGUSR1) ? printf("Signal %d is pending\n", SIGUSR1) : printf("Signal %d is not pending\n", SIGUSR1);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        #ifdef FORK
            printf("Specify what to test from {ignore, handler, mask, pending}\n");
        #endif
        #ifdef EXEC
            printf("Specify what to test from {ignore, mask, pending}\n");
        #endif
        exit(1);
    }

    sigset_t signal_mask;

    if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    #ifdef FORK
        else if (strcmp(argv[1], "handler") == 0) {
            signal(SIGUSR1, handler);
        }
    #endif
    else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        sigemptyset(&signal_mask);
        sigaddset(&signal_mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &signal_mask, NULL);
    }
    else {
        printf("Unrecognised option\n");
        exit(1);
    }

    printf("Raising a signal...\n");
    raise(SIGUSR1);
    check_signal_pending();

#ifdef FORK
    pid_t pid = fork();
    if (pid == -1) {
        printf("Could not fork\n");
        exit(1);
    } else if (pid == 0) {
        if (strcmp(argv[1], "pending") != 0) {
            printf("Raising a signal in child...\n");
            raise(SIGUSR1);
        }
        printf("Child process: ");
        check_signal_pending();
    } else {
        wait(NULL);
    }
    #endif

    #ifdef EXEC
        if (execl("./child", "child", argv[1], NULL) == -1) {
            printf("Could not run sub-program chile, check whether you had compiled it\n");
            exit(1);
        }
    #endif

    return 0;
}