//
// Created by bchwast on 30/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


int caught = 0;
pid_t sender_pid = 0;


void handler_USR1(int signal_id, siginfo_t* info, void* ucontext) {
    caught++;
}

void handler_USR2(int signal_id, siginfo_t* info, void* ucontext) {
    sender_pid = info->si_pid;
}


int correct_mode(char* mode) {
    return  strcmp(mode, "KILL") *
            strcmp(mode, "SIGQUEUE") *
            strcmp(mode, "SIGRT");
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Specify mode\n");
        exit(1);
    }
    if (correct_mode(argv[1]) != 0) {
        printf("Specify one of three modes: {KILL, SIGQUEUE, SIGRT}\n");
        exit(1);
    }

    char* mode = argv[1];

    struct sigaction act_USR1, act_USR2;
    sigemptyset(&act_USR1.sa_mask);
    sigemptyset(&act_USR2.sa_mask);

    act_USR1.sa_flags = act_USR2.sa_flags = SA_SIGINFO;
    act_USR1.sa_sigaction = handler_USR1;
    act_USR2.sa_sigaction = handler_USR2;

    sigaction(strcmp(mode, "SIGRT") == 0 ? SIGRTMIN : SIGUSR1, &act_USR1, NULL);
    sigaction(strcmp(mode, "SIGRT") == 0 ? SIGRTMIN + 1 : SIGUSR2, &act_USR2, NULL);

    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, strcmp(mode, "SIGRT") == 0 ? SIGRTMIN : SIGUSR1);
    sigdelset(&block_mask, strcmp(mode, "SIGRT") == 0 ? SIGRTMIN + 1 : SIGUSR2);
    sigprocmask(SIG_SETMASK, &block_mask, NULL);

    printf("catcher's PID: %d\n", (int) getpid());

    while (!sender_pid);

    printf("Signals received by sender: %d\n", caught);

    union sigval sigval;

    for (int i = 0; i < caught; i++) {
        if (strcmp(mode, "KILL") == 0) {
            kill(sender_pid, SIGUSR1);
        }
        else if (strcmp(mode, "SIGQUEUE") == 0) {
            sigval.sival_int = i;
            sigqueue(sender_pid, SIGUSR1, sigval);
        }
        else {
            kill(sender_pid, SIGRTMIN);
        }
    }

    if (strcmp(mode, "KILL") == 0) {
        kill(sender_pid, SIGUSR2);
    }
    else if (strcmp(mode, "SIGQUEUE") == 0) {
        sigval.sival_int = (int) getpid();
        sigqueue(sender_pid, SIGUSR2, sigval);
    }
    else {
        kill(sender_pid, SIGRTMIN + 1);
    }

    return 0;
}