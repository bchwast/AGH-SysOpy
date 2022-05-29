//
// Created by bchwast on 29/05/22.
//

#include "common.h"

client* clients[MAX_CLIENTS_NUMBER];

int port_num;
char* sock_path;

int sock_fd_local;
struct sockaddr_un sock_struct_local;

int sock_fd_ipv4;
struct sockaddr_in sock_struct_ipv4;

pthread_t thread_connect;
pthread_t thread_ping;
int waiting_client;

game* games[MAX_CLIENTS_NUMBER / 2];
TILE client_signs[MAX_CLIENTS_NUMBER];
int client_games[MAX_CLIENTS_NUMBER];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


int add_game(int player1, int player2) {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (games[i] == NULL) {
            games[i] = create_new_game(player1, player2);
            return i;
        }
    }
    return -1;
}


void make_game(int registered_index) {
    if (waiting_client == -1) {
        printf("No one is waiting\n");
        send_message(clients[registered_index]->fd, game_waiting, NULL);
        waiting_client = registered_index;
    } else {
        printf("Waiting client: %d\n", waiting_client);
        int game_index = add_game(registered_index, waiting_client);

        client_games[registered_index] = game_index;
        client_games[waiting_client] = game_index;

        int coin_toss = rand_range(0, 1);
        int x_ind = coin_toss ? registered_index : waiting_client;
        int o_ind = coin_toss ? waiting_client : registered_index;

        client_signs[x_ind] = X;
        send_message(clients[x_ind]->fd, game_found, "X");
        client_signs[o_ind] = O;
        send_message(clients[o_ind]->fd, game_found, "O");

        waiting_client = -1;
    }
}


void shutdown_server() {
    if (pthread_cancel(thread_connect) == -1) {
        perror("Server error: Could not cancel the connection thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cancel(thread_ping) == -1) {
        perror("Server error: Could not cancel the ping thread\n");
        exit(EXIT_FAILURE);
    }
    if (shutdown(sock_fd_local, SHUT_RDWR) == -1) {
        perror("Server error: Could not shut down the local server\n");
        exit(EXIT_FAILURE);
    }
    if (close(sock_fd_local) == -1) {
        perror("Server error: Could not close the local server\n");
        exit(EXIT_FAILURE);
    }
    if (unlink(sock_path) == -1) {
        perror("Server error: Could not unlink the local server\n");
        exit(EXIT_FAILURE);
    }
    if (shutdown(sock_fd_ipv4, SHUT_RDWR) == -1) {
        perror("Server error: Could not shut down the network server\n");
        exit(EXIT_FAILURE);
    }
    if (close(sock_fd_ipv4) == -1) {
        perror("Server error: Could not close the network server\n");
        exit(EXIT_FAILURE);
    }
}


void sigint_handler() {
    exit(0);
}


void start_local_server() {
    sock_struct_local.sun_family = AF_UNIX;
    strcpy(sock_struct_local.sun_path, sock_path);

    if ((sock_fd_local = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Server error: Could not create local socket\n");
        exit(EXIT_FAILURE);
    }
    if (bind(sock_fd_local, (struct sockaddr*) &sock_struct_local, sizeof(sock_struct_local)) == -1) {
        perror("Server error: Could not bind the local server\n");
        exit(EXIT_FAILURE);
    }
    if (listen(sock_fd_local, MAX_CLIENTS_NUMBER) == -1) {
        perror("Server error: Could not set the local socket as a passive socket\n");
        exit(EXIT_FAILURE);
    }

    printf("UNIX socket listens on %s\n", sock_path);
}


void start_network_server() {
    struct hostent* host_entry = gethostbyname("localhost");
    struct in_addr* host_address = calloc(1, sizeof(struct in_addr));
    host_address = (struct in_addr *) host_entry->h_addr;

    sock_struct_ipv4.sin_family = AF_INET;
    sock_struct_ipv4.sin_port = htons(port_num);
    sock_struct_ipv4.sin_addr.s_addr = host_address->s_addr;

    if ((sock_fd_ipv4 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server error: Could not create network socket\n");
        exit(EXIT_FAILURE);
    }
    if (bind(sock_fd_ipv4, (struct sockaddr*) &sock_struct_ipv4, sizeof(sock_struct_ipv4)) == -1) {
        perror("Server error: Could not bind the network server\n");
        exit(EXIT_FAILURE);
    }
    if (listen(sock_fd_ipv4, MAX_CLIENTS_NUMBER) == -1) {
        perror("Server error: Could not set the network socket as a passive socket\n");
        exit(EXIT_FAILURE);
    }

    printf("INET socket listening on: %s:%d\n", inet_ntoa(*host_address), port_num);
    free(host_address);
}


void close_connection(int fd) {
    if (shutdown(fd, SHUT_RDWR) == -1) {
        perror("Server error: Could not shut down connection\n");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
        perror("Server error: Could not close descriptor\n");
        exit(EXIT_FAILURE);
    }
}


int register_client(int fd, char* name) {
    int free_index = -1;
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0) {
            return -1;
        }
        if (clients[i] == NULL && free_index == -1) {
            free_index = i;
        }
    }
    if (free_index == -1) {
        return -1;
    }
    clients[free_index] = create_client(fd, name);
    return free_index;
}


void unregister_client(int fd) {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i] && clients[i]->fd == fd) {
            clients[i] = NULL;
        }
    }
}


int process_login(int sock_fd) {
    printf("New login pending...\n");

    int client_sock_fd;
    if ((client_sock_fd = accept(sock_fd, NULL, NULL)) == -1) {
        perror("Server error: Could not accept socket descriptor\n");
        exit(EXIT_FAILURE);
    }

    msg* new_msg = read_message(client_sock_fd);
    printf("Received name %s\n", new_msg->data);

    int registered_index = register_client(client_sock_fd, new_msg->data);
    if (registered_index == -1) {
        printf("Login rejected\n");
        send_message(client_sock_fd, login_rejected, "name exists");
        close_connection(client_sock_fd);
    } else {
        printf("Login accepted...\n");
        send_message(client_sock_fd, login_approved, NULL);
    }

    return registered_index;
}


void* process_connections() {
    struct pollfd fds[MAX_CLIENTS_NUMBER + 2];
    fds[MAX_CLIENTS_NUMBER].fd = sock_fd_local;
    fds[MAX_CLIENTS_NUMBER].events = POLLIN;

    fds[MAX_CLIENTS_NUMBER + 1].fd = sock_fd_ipv4;
    fds[MAX_CLIENTS_NUMBER + 1].events = POLLIN;

    waiting_client = -1;
    while (1) {
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            fds[i].fd = clients[i] != NULL ? clients[i]->fd : -1;
            fds[i].events = POLLIN;
            fds[i].revents = 0;
        }
        pthread_mutex_unlock(&clients_mutex);

        fds[MAX_CLIENTS_NUMBER].revents = 0;
        fds[MAX_CLIENTS_NUMBER + 1].revents = 0;

        printf("Pollin...\n");
        poll(fds, MAX_CLIENTS_NUMBER + 2, -1);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS_NUMBER + 2; i++) {
            if (i < MAX_CLIENTS_NUMBER && clients[i] == NULL) {
                continue;
            }

            if (fds[i].revents & POLLHUP) {
                close_connection(fds[i].fd);
                unregister_client(fds[i].fd);
            } else if (fds[i].revents & POLLIN) {
                if (fds[i].fd == sock_fd_local || fds[i].fd == sock_fd_ipv4) {
                    int registered_index = process_login(fds[i].fd);
                    printf("Client registered at index %d\n", registered_index);
                    if (registered_index >= 0) {
                        make_game(registered_index);
                    }
                } else {
                    printf("New message received\n");
                    msg* new_msg = read_message(fds[i].fd);
                    if (new_msg->type == game_move) {
                        printf("Move made: %s\n", new_msg->data);
                        game* g = games[client_games[i]];
                        int field_in = atoi(new_msg->data);
                        TILE sign = client_signs[i];
                        move(g, field_in, sign);
                        int other = g->player1 == i ? g->player2 : g->player1;

                        if (g->info == ONGOING) {
                            send_message(fds[other].fd, game_move, display_board(g));
                        } else if (g->info == DRAW) {
                            send_message(fds[i].fd, game_over, "DRAW");
                            send_message(fds[other].fd, game_over, "DRAW");
                        } else {
                            char* who_won = strdup((g->info == O_WON ? "O WON" : "X WON"));
                            send_message(fds[i].fd, game_over, who_won);
                            send_message(fds[other].fd, game_over, who_won);
                        }
                    } else if (new_msg->type == logout) {
                        close_connection(fds[i].fd);
                        unregister_client(fds[i].fd);
                    } else if (new_msg->type == ping) {
                        clients[i]->is_responding = 1;
                    } else {
                        printf("Server: Unknown message type\n");
                    }
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    pthread_exit((void *) 0);
}


void* process_ping() {
    while (1) {
        sleep(PING_CHECK_GAP);

        printf("Pinging clients\n");

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            if (clients[i] != NULL) {
                clients[i]->is_responding = 0;
                send_message(clients[i]->fd, ping, NULL);
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        sleep(PING_TIMEOUT);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            if (clients[i] != NULL && clients[i]->is_responding == 0) {
                printf("Client %d has not responded. Disconnecting\n", i);
                close_connection(clients[i]->fd);
                unregister_client(clients[i]->fd);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    pthread_exit((void *) 0);
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        perror("Server error: Expected parameters [port number] [socket path]\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    port_num = atoi(argv[1]);
    sock_path = argv[2];

    if (atexit(shutdown_server) == -1) {
        perror("Server error: Could not initialize atexit\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Server error: Could not set signal handler\n");
        exit(EXIT_FAILURE);
    }

    start_local_server();
    start_network_server();

    if (pthread_create(&thread_connect, NULL, process_connections, NULL) == -1) {
        perror("Server error: Could not create connection thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread_ping, NULL, process_ping, NULL) == -1) {
        perror("Server error: Could not create ping thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(thread_connect, NULL) == -1) {
        perror("Server error: Could not join connection thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thread_ping, NULL) == -1) {
        perror("Server error: Could not join ping thread\n");
        exit(EXIT_FAILURE);
    }

    shutdown_server();
    return 0;
}