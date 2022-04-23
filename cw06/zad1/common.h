//
// Created by bchwast on 23/04/22.
//

#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <errno.h>
#include <signal.h>
#include <time.h>

#define MAX_MSG_LEN 256
#define MAX_USERS 20
#define PROJECT_ID 'F'

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5

typedef struct {
    long m_type;
    char m_text[MAX_MSG_LEN];
    pid_t sender_pid;
    int receiver_id;
    struct tm tm;
} msg;

const int size = sizeof(msg) - sizeof(long);

int create_q(const char* path, char proj_id) {
    key_t key = ftok(path, proj_id);
    if (key == -1) {
        perror("Error with ftok\n");
        exit(EXIT_FAILURE);
    }

    int qd = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (qd == -1) {
        perror("Error creating queue\n");
        exit(EXIT_FAILURE);
    }

    return qd;
}

#endif //SYSOPY_COMMON_H
