//
// Created by bchwast on 30/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


int caught = 0;
int done = 0;


void handler_USR1(int signal_id, siginfo_t* info, void* ucontext) {
    caught++;
}

void handler_USR1_queue(int signal_id, siginfo_t* info, void* ucontext) {
    printf("Received signal number %d\n", info->si_value.sival_int);
    caught++;
}

void handler_USR2(int signal_id, siginfo_t* info, void* ucontext) {
    done = 1;
}


int correct_mode(char* mode) {
    return  strcmp(mode, "KILL") *
            strcmp(mode, "SIGQUEUE") *
            strcmp(mode, "SIGRT");
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Expected three arguments [cather's PID] [number of signals to send] [mode]\n");
        exit(1);
    }
    if (correct_mode(argv[3]) != 0) {
        printf("Specify one of three modes: {KILL, SIGQUEUE, SIGRT}\n");
        exit(1);
    }

    pid_t catcher_pid = atoi(argv[1]);
    int n_signals = atoi(argv[2]);
    char* mode = argv[3];

    struct sigaction act_USR1, act_USR2;
    sigemptyset(&act_USR1.sa_mask);
    sigemptyset(&act_USR2.sa_mask);

    act_USR1.sa_flags = act_USR2.sa_flags = SA_SIGINFO;
    act_USR1.sa_sigaction = (strcmp(mode, "SIGQUEUE") == 0 ? handler_USR1_queue : handler_USR1);
    act_USR2.sa_sigaction = handler_USR2;

    sigaction(strcmp(mode, "SIGRT") == 0 ? SIGRTMIN : SIGUSR1, &act_USR1, NULL);
    sigaction(strcmp(mode, "SIGRT") == 0 ? SIGRTMIN + 1 : SIGUSR2, &act_USR2, NULL);

    union sigval sigval;

    for (int i = 0; i < n_signals; i++) {
        if (strcmp(mode, "KILL") == 0) {
            kill(catcher_pid, SIGUSR1);
        }
        else if (strcmp(mode, "SIGQUEUE") == 0) {
            sigval.sival_int = i;
            sigqueue(catcher_pid, SIGUSR1, sigval);
        }
        else {
            kill(catcher_pid, SIGRTMIN);
        }
    }

    if (strcmp(mode, "KILL") == 0) {
        kill(catcher_pid, SIGUSR2);
    }
    else if (strcmp(mode, "SIGQUEUE") == 0) {
        sigval.sival_int = (int) getpid();
        sigqueue(catcher_pid, SIGUSR2, sigval);
    }
    else {
        kill(catcher_pid, SIGRTMIN + 1);
    }

    while (!done);

    printf("Signals sent: %d \nSignals received: %d\n", n_signals, caught);

    return 0;
}