//
// Created by bchwast on 29/05/22.
//

#include "common.h"

char name[MAX_MSG_LEN];
char server_address[MAX_MSG_LEN];
int port_num;

int server_sock_fd;

pthread_t input_thread;
char input[2];

int local_connection;
int logged_in;


void close_connection() {
    send_message(server_sock_fd, logout, NULL, name);

    if (shutdown(server_sock_fd, SHUT_RDWR) == -1) {
        perror("Client error: Could not shutdown\n");
        exit(EXIT_FAILURE);
    }
    if (close(server_sock_fd) == -1) {
        perror("Client error: Could not close connection\n");
        exit(EXIT_FAILURE);
    }
}


void sigint_handler() {
    exit(0);
}


void open_connection() {
    if (local_connection) {
        struct sockaddr_un server_sock;
        server_sock.sun_family = AF_UNIX;
        strcpy(server_sock.sun_path, server_address);

        if ((server_sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
            perror("Client error: Could not create socket\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_un client_sock;
        client_sock.sun_family = AF_UNIX;
        sprintf(client_sock.sun_path, "%s", name);

        if (bind(server_sock_fd, (struct sockaddr *) &client_sock, sizeof(client_sock)) == -1) {
            perror("Client error: Could not bind client\n");
            exit(EXIT_FAILURE);
        }

        if (connect(server_sock_fd, (struct sockaddr *) &server_sock, sizeof(server_sock)) == -1) {
            perror("Client error: Could not connect to the server\n");
            exit(EXIT_FAILURE);
        }
    } else {
        struct sockaddr_in server_sock;
        server_sock.sin_family = AF_INET;
        server_sock.sin_port = htons(port_num);
        server_sock.sin_addr.s_addr = inet_addr(server_address);

        if ((server_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Client error: Could not create socket\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in client_sock;
        client_sock.sin_family = AF_INET;
        client_sock.sin_port = 0;
        client_sock.sin_addr.s_addr = inet_addr(server_address);

        if (bind(server_sock_fd, (struct sockaddr *) &client_sock, sizeof(client_sock)) == -1) {
            perror("Client error: Could not bind client\n");
            exit(EXIT_FAILURE);
        }
        if (connect(server_sock_fd, (struct sockaddr *) &server_sock, sizeof(server_sock)) == -1) {
            perror("Client error: Could not connect to the server\n");
            exit(EXIT_FAILURE);
        }
    }

    logged_in = 1;
}


void* thread_read_input() {
    printf("Enter your next move: ");
    scanf("%s", input);

    pthread_exit((void *) 0);
}


void enter_next_move() {
    strcpy(input, "w");
    if (pthread_create(&input_thread, NULL, thread_read_input, NULL) == -1) {
        perror("Client error: Could not initialize read thread\n");
        exit(EXIT_FAILURE);
    }

    msg* message;
    while (strcmp(input, "w") == 0) {
        message = read_message_noblock(server_sock_fd);
        if (message != NULL) {
            if(message->type == ping) {
                printf("Ping check\n");
                send_message(server_sock_fd, ping, NULL, name);
            }
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        perror("Client error: Expected parameters [name] [connection method] [server address/\"local\"] [port number (if server)]\n");
        exit(EXIT_FAILURE);
    }

    strcpy(name, argv[1]);
    local_connection = (strcmp(argv[2], "local") == 0 ) ? 1 : 0;
    strcpy(server_address, argv[3]);

    if (!local_connection) {
        if (argc > 4) {
            port_num = atoi(argv[4]);
        } else {
            perror("Client error: Too few parameters\n");
            exit(EXIT_FAILURE);
        }
    }

    if (atexit(close_connection) == -1) {
        perror("Client error: Could not initialize atexit\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Client error: Could not set SIGINT handler\n");
        exit(EXIT_FAILURE);
    }

    open_connection();
    send_message(server_sock_fd, login_request, NULL,name);
    msg* answer = read_message(server_sock_fd);

    if (answer->type == login_approved) {
        printf("You are successfully registered\n");
        while (1) {
            answer = read_message(server_sock_fd);

            if (answer->type == game_waiting) {
                printf("Waiting for a game\n");
            } else if (answer->type == game_found) {
                TILE players_sign = strcmp("X", answer->data) == 0 ? X : O;
                printf("Found game. You play as %s\n", answer->data);

                if (players_sign == X) {
                    enter_next_move();
                    printf("Your move: %s\n", input);
                    send_message(server_sock_fd, game_move, input, name);
                }

                while (1) {
                    answer = read_message(server_sock_fd);

                    if (answer->type == game_move) {
                        printf("Opponent made a move. Board:\n");
                        printf("%s", answer->data);
                        enter_next_move();
                        printf("Your move: %s\n", input);
                        send_message(server_sock_fd, game_move, input, name);
                    } else if (answer->type == game_over) {
                        printf("Game over. %s\n", answer->data);
                        send_message(server_sock_fd, logout, NULL, name);
                        printf("Logged out of server\n");
                        logged_in = 0;
                        break;
                    } else if (answer->type == ping) {
                        printf("Ping check\n");
                        send_message(server_sock_fd, ping, NULL, name);
                    } else {
                        perror("Client error: Unknown message type received\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            } else if (answer->type == ping) {
                printf("Ping check\n");
                send_message(server_sock_fd, ping, NULL, name);
            }
        }
    } else if (answer->type == login_rejected) {
        printf("Server rejected your login. Reason: %s", answer->data);
    } else {
        printf("Unknown message type\n");
    }

    if (logged_in) {
        close_connection();
    }

    return 0;
}