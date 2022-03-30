//
// Created by bchwast on 30/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>


void handler_INT(int signal_id, siginfo_t* info, void* ucontext) {
    printf("\tReceived SIGINT signal: %d, from process: %d\n", info->si_signo, info->si_pid);
    printf("\tUID of process' owner: %d, signal code: %d, errno number: %d\n\n", info->si_uid, info->si_code, info->si_errno);
    exit(0);
}

void handler_ABRT(int signal_id, siginfo_t* info, void* ucontext) {
    printf("\tReceived SIGABRT signal: %d, from process: %d\n", info->si_signo, info->si_pid);
    printf("\tUID of process' owner: %d, signal code: %d, errno number: %d\n\n", info->si_uid, info->si_code, info->si_errno);
    exit(0);
}

void handler_SEGV(int signal_id, siginfo_t* info, void* ucontext) {
    printf("\tReceived SIGSEGV signal: %d, from process: %d\n", info->si_signo, info->si_pid);
    printf("\tUID of process' owner: %d, signal code: %d, errno number: %d\n\n", info->si_uid, info->si_code, info->si_errno);
    exit(0);
}

void handler_CONT(int signal_id, siginfo_t* info, void* ucontext) {
    printf("\tHello there, I am SIGCONT handler, I have a crippling depression\n\n");
}

int cnt = 3;
void recursive_handler(int signal_id, siginfo_t* info, void* ucontext) {
    int rec_call_num = 4 - cnt;
    printf("\tStart recursive call number %d\n", rec_call_num);
    cnt--;

    if (cnt) {
        raise(signal_id);
    }

    printf("\tStop recursive call number %d\n", rec_call_num);
}


void setup_sigaction(struct sigaction act, int signal_id, int flags, void (*action)(int, siginfo_t*, void*)) {
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = action;
    act.sa_flags = flags;
    sigaction(signal_id, &act, NULL);
}


void siginfo() {
    printf("### SA_SIGINFO ###\n");
    pid_t child_pid;

    child_pid = fork();
    if (child_pid == -1) {
        printf("Could not fork\n");
        exit(0);
    }
    else if (child_pid == 0) {
        struct sigaction act_INT;
        setup_sigaction(act_INT, SIGINT, SA_SIGINFO, handler_INT);
        raise(SIGINT);
    }
    else {
        wait(NULL);
    }

    child_pid = fork();
    if (child_pid == -1) {
        printf("Could not fork\n");
        exit(0);
    }
    else if (child_pid == 0) {
        struct sigaction act_ABRT;
        setup_sigaction(act_ABRT, SIGABRT, SA_SIGINFO, handler_ABRT);
        raise(SIGABRT);
    }
    else {
        wait(NULL);
    }

    child_pid = fork();
    if (child_pid == -1) {
        printf("Could not fork\n");
        exit(0);
    }
    else if (child_pid == 0) {
        struct sigaction act_SEGV;
        setup_sigaction(act_SEGV, SIGSEGV, SA_SIGINFO, handler_SEGV);
        raise(SIGSEGV);
    }
    else {
        wait(NULL);
    }
}


void resethand() {
    printf("### SA_RESETHAND ###\n");
    pid_t child_pid;

    child_pid = fork();
    if (child_pid == -1) {
        printf("Could not fork\n");
        exit(0);
    }
    else if (child_pid == 0) {
        struct sigaction act;
        setup_sigaction(act, SIGCONT, SA_RESETHAND, handler_CONT);
        raise(SIGCONT);
        raise(SIGCONT);
        raise(SIGCONT);
        exit(0);
    }
    else {
        wait(NULL);
    }
}


void nodefer() {
    printf("### SA_NODEFER ###\n");
    pid_t child_pid;

    child_pid = fork();
    if (child_pid == -1) {
        printf("Could not fork\n");
        exit(0);
    }
    else if (child_pid == 0) {
        struct sigaction act;
        setup_sigaction(act, SIGCONT, SA_NODEFER, recursive_handler);
        raise(SIGCONT);
        exit(0);
    }
    else {
        wait(NULL);
    }
}


int main() {
    siginfo();
    resethand();
    nodefer();

    return 0;
}