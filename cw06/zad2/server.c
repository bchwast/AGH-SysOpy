//
// Created by bchwast on 23/04/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "common.h"
#include <mqueue.h>
#include <sys/stat.h>


mqd_t qd;
int users_pid[MAX_USERS];
mqd_t users_q_pid[MAX_USERS];
char msg_log[MAX_MSG_LEN];
FILE* fd_log;


void close_q() {
    fprintf(fd_log, "closing\n");
    fclose(fd_log);
    for (int i = 0; i < MAX_USERS; i++) {
        if (users_pid[i] != 0) {
            mq_close(users_q_pid[i]);
            kill(users_pid[i], SIGINT);
        }
    }
    mq_unlink(SERVER_Q);
    exit(EXIT_SUCCESS);
}


void log_msg(msg* msg_buff, uint m_type) {
    char buff[300];
    strcpy(buff, "");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    switch (m_type) {
        case STOP:
            fprintf(fd_log, "STOP %d\n", msg_buff->sender_pid);
            break;
        case LIST:
            fprintf(fd_log, "LIST requested by %d\n", msg_buff->sender_pid);
            break;
        case TO_ALL:
            fprintf(fd_log, "TO_ALL from %d\n", msg_buff->sender_pid);
            fprintf(fd_log, "%s\n", msg_buff->m_text);
            break;
        case TO_ONE:
            fprintf(fd_log, "TO_ONE from %d to %d\n", msg_buff->sender_pid, msg_buff->receiver_id);
            fprintf(fd_log, "%s\n", msg_buff->m_text);
            break;
        case INIT:
            fprintf(fd_log, "INIT %d\n", msg_buff->sender_pid);
            break;
    }

    fprintf(fd_log, "\n%d-%02d-%02d %02d:%02d:%02d\n\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}


void init(msg* msg_buff) {
    int i = 0;
    for (; i < MAX_USERS; i++) {
        if (users_pid[i] == 0) {
            users_pid[i] = msg_buff->sender_pid;
            users_q_pid[i] = open_q(msg_buff->m_text);
            break;
        }
    }

    msg msg_;
    msg_.sender_pid = getpid();
    sprintf(msg_.m_text, "%d", i);

    if (mq_send(users_q_pid[i], (char *) &msg_, size, INIT) == -1) {
        perror("Server error: could not send INIT message\n");
        exit(EXIT_FAILURE);
    }
}


void msg_to_one(msg* msg_buff) {
    time_t tm = time(NULL);
    msg_buff->tm = *localtime(&tm);

    if (mq_send(users_q_pid[msg_buff->receiver_id], (char *) msg_buff, size, TO_ONE) == -1) {
        perror("Server error: could not send TO_ONE message\n");
        exit(EXIT_FAILURE);
    }
    kill(users_pid[msg_buff->receiver_id], SIGUSR1);
}


void msg_to_all(msg* msg_buff) {
    time_t tm = time(NULL);
    msg_buff->tm = *localtime(&tm);

    for (int i = 0; i < MAX_USERS; i++) {
        if (users_pid[i] != 0 && i != msg_buff->sender_pid) {
            if (mq_send(users_q_pid[i], (char *) msg_buff, size, TO_ALL) == -1) {
                perror("Server error: could not send TO_ALL message\n");
                exit(EXIT_FAILURE);
            }
            kill(users_pid[i], SIGUSR1);
        }
    }
}


void list(msg* msg_buff) {
    printf("----list of users----\n");
    for (int i = 0; i < MAX_USERS; i++) {
        if (users_pid[i] != 0) {
            printf("- %d", i);
            if (i == msg_buff->sender_pid) {
                printf(" that's you");
            }
            printf("\n");
        }
    }
    printf("----------------------\n");
}


void disconnect(msg* msg_buff) {
    int i = msg_buff->sender_pid;
    users_pid[i] = 0;
    users_q_pid[i] = 0;
}


void process_message(msg* msg_buff, uint m_type) {
    switch (m_type) {
        case STOP:
            disconnect(msg_buff);
            break;
        case LIST:
            list(msg_buff);
            break;
        case TO_ALL:
            msg_to_all(msg_buff);
            break;
        case TO_ONE:
            msg_to_one(msg_buff);
            break;
        case INIT:
            init(msg_buff);
    }
    log_msg(msg_buff, m_type);
}


int main() {
    if (signal(SIGINT, close_q) == SIG_ERR) {
        perror("Server error: problem with SIGINT\n");
        exit(EXIT_FAILURE);
    }

    strcpy(msg_log, "server_logs.txt");
    if ((fd_log = fopen(msg_log, "w")) == NULL) {
        perror("Server error: cannot open server_logs.txt\n");
        exit(EXIT_FAILURE);
    }

    qd = create_q(SERVER_Q);
    msg msg_buff;
    uint m_type;

    while (1) {
        if (mq_receive(qd, (char *) &msg_buff, size, &m_type) == -1) {
            perror("Server error: could not receive message\n");
            exit(EXIT_FAILURE);
        }

        process_message(&msg_buff, m_type);
    }
}