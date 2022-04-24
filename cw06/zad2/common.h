//
// Created by bchwast on 24/04/22.
//

#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>

#include <mqueue.h>
#include <sys/types.h>

#include <errno.h>
#include <signal.h>
#include <time.h>

#define MAX_MSG_LEN 256
#define MAX_USERS 20
#define PROJECT_ID 'F'
#define SERVER_Q "/server_q"

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5

typedef struct {
    char m_text[MAX_MSG_LEN];
    pid_t sender_pid;
    int receiver_id;
    struct tm tm;
} msg;

const int size = sizeof(msg);

mqd_t create_q(const char* path) {
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = size;

    mqd_t qd = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    if (qd == -1) {
        perror("Error creating queue\n");
        exit(EXIT_FAILURE);
    }

    return qd;
}


mqd_t open_q(const char* path) {
    mqd_t qd = mq_open(path, O_WRONLY);
    if (qd == -1) {
        perror("Error: could not open queue\n");
        exit(EXIT_FAILURE);
    }
    return qd;
}

#endif //SYSOPY_COMMON_H
