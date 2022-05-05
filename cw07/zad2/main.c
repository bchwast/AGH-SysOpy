//
// Created by bchwast on 05/05/22.
//

#include "common.h"


pid_t cooks[MAX_COOKS];
pid_t deliverers[MAX_DELIVERERS];
int n_cooks;
int n_deliverers;
int shm_fd;
const char* sem_names[NO_SEMAPHORES] = {"/IS_OVEN_USED", "/OVEN_FULL_COUNT","/IS_TABLE_USED", "/TABLE_EMPTY_COUNT", "/TABLE_FULL_COUNT"};


void sigint_handler(int signal) {
    exit(0);
}


void before_closing() {
    for (int i = 0; i < n_cooks; i++) {
        kill(cooks[i], SIGINT);
    }
    for (int i = 0; i < n_deliverers; i++) {
        kill(deliverers[i], SIGINT);
    }

    for (int i = 0; i < NO_SEMAPHORES; i++) {
        if (sem_unlink(sem_names[i]) == -1) {
            perror("Error: Could not remove semaphore\n");
            exit(EXIT_FAILURE);
        }
    }
    if (shm_unlink(SHARED_MEMORY) == -1) {
        perror("Error: Could not remove shared memory\n");
        exit(EXIT_FAILURE);
    }
}


void init_sems() {
    int init_values[NO_SEMAPHORES] = {1, MAX_OVEN_SIZE, 1, 0, MAX_TABLE_SIZE};

    for (int i = 0; i < NO_SEMAPHORES; i++) {
        sem_t* sem = sem_open(sem_names[i], O_RDWR | O_CREAT | O_EXCL, 0666, init_values[i]);
        if (sem == SEM_FAILED) {
            perror("Error: Could not create semaphore\n");
            exit(EXIT_FAILURE);
        }
        sem_close(sem);
    }
}


void init_shm() {
    shm_fd = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (shm_fd == -1) {
        perror("Error: Could not get shared memory %d\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(oven_and_table)) == -1) {
        perror("Error: Could not truncate shared memory\n");
        exit(EXIT_FAILURE);
    }

    oven_and_table* o_t = save_shm_attach(shm_fd);
    if (o_t == NULL) {
        perror("Error: Could not get shared memory\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_OVEN_SIZE; i++) {
        o_t->oven[i] = -1;
    }
    for (int i = 0; i < MAX_TABLE_SIZE; i++) {
        o_t->table[i] = -1;
    }

    o_t->last_pizza = 0;
    o_t->n_pizzas_oven = 0;
    o_t->taken_pizza = 0;
    o_t->n_pizzas_table = 0;
    safe_shm_detach(o_t);
}


void fork_cook(int i) {
    pid_t cook_pid = fork();
    if (cook_pid == 0) {
        execlp("./cook", "cook", NULL);
    }
    cooks[i] = cook_pid;
}


void fork_deliverer(int i) {
    pid_t deliverer_pid = fork();
    if (deliverer_pid == 0) {
        execlp("./deliverer", "deliverer", NULL);
    }
    deliverers[i] = deliverer_pid;
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        perror("Error: Expected 2 arguments\n");
        exit(EXIT_FAILURE);
    }

    n_cooks = atoi(argv[1]);
    n_deliverers = atoi(argv[2]);
    if (n_cooks > MAX_COOKS || n_deliverers > MAX_DELIVERERS) {
        perror("Error: Too many cooks or deliverers\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error: Could not set signal handler\n");
        exit(EXIT_FAILURE);
    }
    if (atexit(before_closing) == -1) {
        perror("Error: Could not set atexit\n");
        exit(EXIT_FAILURE);
    }

    init_sems();
    init_shm();

    for (int i = 0; i < n_cooks; i++) {
        fork_cook(i);
    }
    for (int i = 0; i < n_deliverers; i++) {
        fork_deliverer(i);
    }

    while (wait(NULL) != -1);

    return 0;
}