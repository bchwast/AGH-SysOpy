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
        send_message_to(clients[registered_index]->fd, game_waiting, NULL, clients[registered_index]->addr);
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
        send_message_to(clients[x_ind]->fd, game_found, "X", clients[x_ind]->addr);
        client_signs[o_ind] = O;
        send_message_to(clients[o_ind]->fd, game_found, "O", clients[o_ind]->addr);

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
    if (close(sock_fd_local) == -1) {
        perror("Server error: Could not close the local server\n");
        exit(EXIT_FAILURE);
    }
    if (unlink(sock_path) == -1) {
        perror("Server error: Could not unlink the local server\n");
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

    if ((sock_fd_local = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("Server error: Could not create local socket\n");
        exit(EXIT_FAILURE);
    }
    if (bind(sock_fd_local, (struct sockaddr*) &sock_struct_local, sizeof(sock_struct_local)) == -1) {
        perror("Server error: Could not bind the local server\n");
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

    if ((sock_fd_ipv4 = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Server error: Could not create network socket\n");
        exit(EXIT_FAILURE);
    }
    if (bind(sock_fd_ipv4, (struct sockaddr*) &sock_struct_ipv4, sizeof(sock_struct_ipv4)) == -1) {
        perror("Server error: Could not bind the network server\n");
        exit(EXIT_FAILURE);
    }

    printf("INET socket listening on: %s:%d\n", inet_ntoa(*host_address), port_num);
    free(host_address);
}


int register_client(int fd, struct sockaddr* addr, char* name) {
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
    clients[free_index] = create_client(fd, addr, name);
    return free_index;
}


void unregister_client(char* name) {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i] && strcmp(clients[i]->name, name) == 0) {
            clients[i] = NULL;
        }
    }
}


int get_user_index(char* name) {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}


void* process_connections() {
    struct pollfd fds[2];
    fds[0].fd = sock_fd_local;
    fds[0].events = POLLIN;

    fds[1].fd = sock_fd_ipv4;
    fds[1].events = POLLIN;

    waiting_client = -1;
    while (1) {
        for (int i = 0; i < 2; i++) {
            fds[i].events = POLLIN;
            fds[i].revents = 0;
        }
        printf("Pollin...\n");
        poll(fds, 2, -1);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < 2; i++) {
            if (fds[i].revents & POLLIN) {
                printf("New message received\n");
                struct sockaddr* addr = (struct sockaddr *) calloc(1, sizeof(struct sockaddr));
                socklen_t len = sizeof(&addr);
                msg* new_msg = read_message_from(fds[i].fd, addr, &len);
                if (new_msg->type == login_request) {
                    printf("Registering user. Name %s\n", new_msg->user);
                    int registered_index = register_client(fds[i].fd, addr, new_msg->user);
                    printf("Client registered at index %d\n", registered_index);
                    if (registered_index == -1) {
                        send_message_to(fds[i].fd, login_rejected, "name already in use", addr);
                    } else {
                        send_message_to(fds[i].fd, login_approved, NULL, addr);
                        make_game(registered_index);
                    }
                } else if (new_msg->type == logout) {
                    unregister_client(new_msg->user);
                    printf("User %s logged out\n", new_msg->user);
                } else if (new_msg->type == game_move) {
                    printf("Move made: %s\n", new_msg->data);
                    int i = get_user_index(new_msg->user);
                    game* g = games[client_games[i]];
                    int field_in = atoi(new_msg->data);
                    TILE sign = client_signs[i];
                    move(g, field_in, sign);
                    int other = g->player1 == i ? g->player2 : g->player1;

                    if (g->info == ONGOING) {
                        send_message_to(clients[other]->fd, game_move, display_board(g), clients[other]->addr);
                    } else if (g->info == DRAW) {
                        send_message_to(clients[i]->fd, game_over, "DRAW", clients[i]->addr);
                        send_message_to(clients[other]->fd, game_over, "DRAW", clients[other]->addr);
                    } else {
                        char* who_won = strdup((g->info == O_WON ? "O WON" : "X WON"));
                        send_message_to(clients[i]->fd, game_over, who_won, clients[i]->addr);
                        send_message_to(clients[other]->fd, game_over, who_won, clients[other]->addr);
                    }
                } else if (new_msg->type == ping) {
                    int i = get_user_index(new_msg->user);
                    clients[i]->is_responding = 1;
                } else {
                    printf("Server: Unknown message type\n");
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
                send_message_to(clients[i]->fd, ping, NULL, clients[i]->addr);
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        sleep(PING_TIMEOUT);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            if (clients[i] != NULL && clients[i]->is_responding == 0) {
                printf("Client %d has not responded. Disconnecting\n", i);
                unregister_client(clients[i]->name);
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