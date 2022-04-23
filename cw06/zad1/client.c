//
// Created by bchwast on 23/04/22.
//

#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "common.h"

int qd;
int my_q;
int my_id;


void receive_msg() {
    msg msg_;
    if (msgrcv(my_q, &msg_, size, -INIT - 1, 0) == -1) {
        perror("Client error: could not receive a message from server\n");
        exit(EXIT_FAILURE);
    }

    struct tm tm = msg_.tm;

    printf("> New message from ID: %d\n%s\n\n%d-%02d-%02d %02d:%02d:%02d\n", msg_.sender_pid, msg_.m_text,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}


void send_to_one(int receiver_id, char* text) {
    msg msg_;
    msg_.m_type = TO_ONE;
    msg_.sender_pid = my_id;
    msg_.receiver_id = receiver_id;
    strcpy(msg_.m_text, text);

    if (msgsnd(qd, &msg_, size, 0) == -1) {
        perror("Client error: could not send TO_ONE message to server\n");
        exit(EXIT_FAILURE);
    }
}


void send_to_all(char* text) {
    msg msg_;
    msg_.m_type = TO_ALL;
    msg_.sender_pid = my_id;
    strcpy(msg_.m_text, text);

    if (msgsnd(qd, &msg_, size, 0) == -1) {
        perror("Client error: could not send TO_ALL message to server\n");
        exit(EXIT_FAILURE);
    }
}


void send_list() {
    msg msg_;
    msg_.m_type = LIST;
    msg_.sender_pid = my_id;
    msg_.m_text[0] = '\0';

    if (msgsnd(qd, &msg_, size, 0) == -1) {
        perror("Client error: could not send LIST message to server\n");
        exit(EXIT_FAILURE);
    }
}


void die() {
    msgctl(my_q, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}


void stop() {
    msgctl(my_q, IPC_RMID, NULL);

    msg msg_;
    msg_.m_type = STOP;
    msg_.sender_pid = my_id;

    if (msgsnd(qd, &msg_, size, 0) == -1) {
        perror("Client error: could not send STOP message to server\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


void send_init() {
    msg to_server;
    to_server.m_type = INIT;
    to_server.sender_pid = getpid();
    sprintf(to_server.m_text, "%d", my_q);

    if (msgsnd(qd, &to_server, size, 0) == -1) {
        perror("Client error: could not send INIT message to server\n");
        exit(EXIT_FAILURE);
    }

    msg from_server;

    if (msgrcv(my_q, &from_server, size, INIT, 0) == -1) {
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

    char* home_path = getenv("HOME");
    if (home_path == NULL) {
        perror("Client error: could not get home_path\n");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(home_path, PROJECT_ID);
    if (key == -1) {
        perror("Client error: could not get the queue key\n");
        exit(EXIT_FAILURE);
    }

    qd = msgget(key, 0);
    if (qd == -1) {
        perror("Client error: could not get the server queue\n");
        exit(EXIT_FAILURE);
    }

    my_q = create_q(home_path, (getpid() % ('z' - 'a' + 1)) + 'a');

    send_init();

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