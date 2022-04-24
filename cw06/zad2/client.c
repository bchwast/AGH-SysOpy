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

mqd_t qd;
mqd_t my_q;
int my_id;
char q_path[250];


void receive_msg() {
    msg msg_;
    uint m_type;
    if (mq_receive(my_q, (char *) &msg_, size, &m_type) == -1) {
        perror("Client error: could not receive a message from server\n");
        exit(EXIT_FAILURE);
    }

    struct tm tm = msg_.tm;

    printf("> New message from ID: %d\n%s\n\n%d-%02d-%02d %02d:%02d:%02d\n", msg_.sender_pid, msg_.m_text,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}


void send_to_one(int receiver_id, char* text) {
    msg msg_;
    msg_.sender_pid = my_id;
    msg_.receiver_id = receiver_id;
    strcpy(msg_.m_text, text);

    if (mq_send(qd, (char *) &msg_, size, TO_ONE) == -1) {
        perror("Client error: could not send TO_ONE message to server\n");
        exit(EXIT_FAILURE);
    }
}


void send_to_all(char* text) {
    msg msg_;
    msg_.sender_pid = my_id;
    strcpy(msg_.m_text, text);

    if (mq_send(qd, (char *) &msg_, size, TO_ALL) == -1) {
        perror("Client error: could not send TO_ALL message to server\n");
        exit(EXIT_FAILURE);
    }
}


void send_list() {
    msg msg_;
    msg_.sender_pid = my_id;
    msg_.m_text[0] = '\0';

    if (mq_send(qd, (char *) &msg_, size, LIST) == -1) {
        perror("Client error: could not send LIST message to server\n");
        exit(EXIT_FAILURE);
    }
}


void die() {
    mq_close(qd);
    mq_unlink(q_path);
    exit(EXIT_SUCCESS);
}


void stop() {
    msg msg_;
    msg_.sender_pid = my_id;
    msg_.m_text[0] = '\0';

    if (mq_send(qd, (char *) &msg_, size, STOP) == -1) {
        perror("Client error: could not send STOP message to server\n");
        exit(EXIT_FAILURE);
    }

    mq_close(qd);
    mq_unlink(q_path);

    exit(EXIT_SUCCESS);
}


void send_init(const char* path) {
    msg to_server;
    to_server.sender_pid = getpid();
    sprintf(to_server.m_text, "%s", path);

    if (mq_send(qd, (char *) &to_server, size, INIT) == -1) {
        perror("Client error: could not send INIT message to server\n");
        exit(EXIT_FAILURE);
    }

    msg from_server;

    if (mq_receive(my_q, (char *) &from_server, size, NULL) == -1) {
        perror("Client error: could not receive INIT message from server\n");
        exit(EXIT_FAILURE);
    }

    my_id = atoi(from_server.m_text);
    fflush(stdout);
}


int main() {
    if (atexit(die) != 0) {
        printf("Client error: problem with atexit\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, stop) == SIG_ERR) {
        printf("Client error: problem with SIGINT handler\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR1, receive_msg) == SIG_ERR) {
        printf("Client error: problem with SIGUSR1 handler\n");
        exit(EXIT_FAILURE);
    }

    sprintf((char *) q_path, "/%d", getpid());

    qd = open_q(SERVER_Q);

    my_q = create_q(q_path);

    send_init(q_path);

    char input[MAX_MSG_LEN];
    char* token;
    char text[MAX_MSG_LEN];

    while (1) {
        fgets(input, MAX_MSG_LEN, stdin);
        input[strcspn(input, "\n")] = 0;
        token = strtok(input, " ");

        if (strcmp(token, "STOP") == 0) {
            stop();
        } else if (strcmp(token, "LIST") == 0) {
            send_list();
        } else if (strcmp(token, "2ALL") == 0) {
            token = strtok(NULL, "\n");
            sprintf(text, "%s", token);
            send_to_all(text);
        } else if (strcmp(token, "2ONE") == 0) {
            int receiver_id = -1;
            token = strtok(NULL, " ");
            receiver_id = atoi(token);
            token = strtok(NULL, "\n");
            sprintf(text, "%s", token);
            send_to_one(receiver_id, text);
        }
    }
}