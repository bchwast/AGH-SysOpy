//
// Created by bchwast on 28/05/22.
//

#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#endif //SYSOPY_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <endian.h>

#include <poll.h>
#include <netdb.h>

#define MAX_CLIENTS_NUMBER 15
#define MAX_MSG_LEN 20
#define PING_TIMEOUT 10
#define PING_CHECK_GAP 20


typedef enum TILE {
    O = -1,
    X = 1,
    EMPTY = 0
} TILE;


typedef enum GAME_INFO {
    ONGOING,
    DRAW,
    O_WON,
    X_WON,
} GAME_INFO;


typedef struct game {
    int player1;
    int player2;
    TILE grid[9];
    GAME_INFO info;
} game;


game* create_new_game(int player1, int player2) {
    game* g = (game *) calloc(1, sizeof(game));
    g->player1 = player1;
    g->player2 = player2;
    for (int i = 0; i < 9; i++) {
        g->grid[i] = EMPTY;
    }
    g->info = ONGOING;

    return g;
}


GAME_INFO check_game_info(game* g) {
    int possible_wins[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8},
                               {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
                               {0, 4, 8}, {2, 4, 6}};
    for (int i = 0; i < 8; i++) {
        int win_sum_check = 0;
        for (int j = 0; j < 3; j++) {
            win_sum_check += g->grid[possible_wins[i][j]];
        }
        if (win_sum_check == -3) {
            return O_WON;
        }
        if (win_sum_check == 3) {
            return X_WON;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (g->grid[i] == EMPTY) {
            return ONGOING;
        }
    }
    return DRAW;
}


void move(game* g, int field_index, TILE character) {
    g->grid[field_index] = character;
    g->info = check_game_info(g);
}


char* display_board(game* g) {
    char* result = (char *) calloc(13, sizeof(char));
    int curr_ind = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int tile_num_key = 3*i + j;
            TILE sym = g->grid[tile_num_key];
            char sign_char_representation = sym == X ? 'X' : (sym == O ? 'O' : '0' + tile_num_key);
            result[curr_ind++] = sign_char_representation;
        }
        result[curr_ind++] = '\n';
    }
    return result;
}


typedef enum MSG_TYPE {
    login_request,
    login_approved,
    login_rejected,
    game_waiting,
    game_found,
    game_move,
    game_over,
    logout,
    ping
} MSG_TYPE;


typedef struct msg {
    MSG_TYPE type;
    char data[MAX_MSG_LEN];
} msg;


typedef struct client {
    int fd;
    char name[MAX_MSG_LEN];
    int is_responding;
} client;


msg* read_message(int sock_fd) {
    msg* result_msg = (msg *) calloc(1, sizeof(msg));
    char* raw_msg = (char *) calloc(MAX_MSG_LEN, sizeof(char));
    if (read(sock_fd, (void *) raw_msg, MAX_MSG_LEN) < 0) {
        perror("Error: Could not read the message\n");
        exit(EXIT_FAILURE);
    }
    result_msg->type = atoi(strtok(raw_msg, ":"));
    strcpy(result_msg->data, strtok(NULL, ":"));
    free(raw_msg);
    return result_msg;
}


msg* read_message_noblock(int sock_fd) {
    msg* result_msg = (msg *) calloc(1, sizeof(msg));
    char* raw_msg = (char *) calloc(MAX_MSG_LEN, sizeof(char));
    if (recv(sock_fd, (void *) raw_msg, MAX_MSG_LEN, MSG_DONTWAIT) < 0) {
        return NULL;
    }
    result_msg->type = atoi(strtok(raw_msg, ":"));
    strcpy(result_msg->data, strtok(NULL, ":"));
    free(raw_msg);
    return result_msg;
}


void send_message(int sock_fd, MSG_TYPE type, char* data) {
    char* raw_msg = (char *) calloc(MAX_MSG_LEN, sizeof(char));
    sprintf(raw_msg, "%d:%s", (int) type, data);
    if (write(sock_fd, (void *) raw_msg, MAX_MSG_LEN) == -1) {
        perror("Error: Could not send message\n");
        exit(EXIT_FAILURE);
    }
    free(raw_msg);
}


client* create_client(int fd, char* name) {
    client* c = (client *) calloc(1, sizeof(client));
    c->fd = fd;
    c->is_responding = 1;
    strcpy(c->name, name);
    return c;
}


int rand_range(int a, int b) {
    return rand() % (b - a + 1) + a;
}