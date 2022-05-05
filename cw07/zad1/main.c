//
// Created by bchwast on 05/05/22.
//

#include "common.h"


pid_t cooks[MAX_COOKS];
pid_t deliverers[MAX_DELIVERERS];
int n_cooks;
int n_deliverers;
int sem_id;
int shm_id;


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

    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Error: Could not remove semaphore\n");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("Error: Could not remove shared memory\n");
        exit(EXIT_FAILURE);
    }
}


void init_sems() {
    key_t sem_key = ftok(get_home_path(), 0);
    sem_id = semget(sem_key, 5, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_id == -1) {
        perror("Error: Could not get semaphore\n");
        exit(EXIT_FAILURE);
    }

    sem_init_value(sem_id, IS_OVEN_USED, 1);
    sem_init_value(sem_id, OVEN_FULL_COUNT, MAX_OVEN_SIZE);

    sem_init_value(sem_id, IS_TABLE_USED, 1);
    sem_init_value(sem_id, TABLE_EMPTY_COUNT, 0);
    sem_init_value(sem_id, TABLE_FULL_COUNT, MAX_TABLE_SIZE);
}


void init_shm() {
    key_t shm_key = ftok(get_home_path(), 1);
    shm_id = shmget(shm_key, sizeof(oven_and_table), IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1) {
        perror("Error: Could not get shared memory %d\n");
        exit(EXIT_FAILURE);
    }

    oven_and_table* o_t = shmat(shm_id, NULL, 0);
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
    shmdt(o_t);
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