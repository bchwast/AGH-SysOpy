//
// Created by bchwast on 05/05/22.
//

#ifndef SYSOPY_COMMON_H
#define SYSOPY_COMMON_H

#include <stdlib.h>
#include <pwd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_DELIVERERS 30
#define MAX_COOKS 30
#define MAX_OVEN_SIZE 5
#define MAX_TABLE_SIZE 5

#define IS_OVEN_USED 0
#define OVEN_FULL_COUNT 1
#define IS_TABLE_USED 2
#define TABLE_EMPTY_COUNT 3
#define TABLE_FULL_COUNT 4


typedef struct oven_and_table {
    int oven[MAX_OVEN_SIZE];
    int table[MAX_TABLE_SIZE];

    int last_pizza;
    int n_pizzas_oven;

    int taken_pizza;
    int n_pizzas_table;
} oven_and_table;


union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* _buff;
};


char* get_home_path() {
    char* path = getenv("HOME");
    if (path == NULL) {
        path = getpwuid(getuid())->pw_dir;
    }
    return path;
}


int get_semaphore() {
    key_t sem_key = ftok(get_home_path(), 0);
    int sem_id = semget(sem_key, 0, 0);
    if (sem_id == -1) {
        perror("Error: Could not get semaphore\n");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}


int get_shared_memory() {
    key_t shm_key = ftok(get_home_path(), 1);
    int shm_id = shmget(shm_key, 0, 0);
    if (shm_id == -1) {
        perror("Error: Could not get shared memory\n");
        exit(EXIT_FAILURE);
    }
    return shm_id;
}


void sem_wait(int sem_id, int sem_num) {
    struct sembuf buf = {sem_num, -1, 0};
    if (semop(sem_id, &buf, 1) == -1) {
        perror("Error: Could not make semaphore wait\n");
        exit(EXIT_FAILURE);
    }
}


void sem_signal(int sem_id, int sem_num) {
    struct sembuf buf = {sem_num, 1, 0};
    if (semop(sem_id, &buf, 1) == -1) {
        perror("Error: Could not signal semaphore\n");
        exit(EXIT_FAILURE);
    }
}


void sem_init_value(int sem_id, int sem_num, int value) {
    union semun arg;
    arg.val = value;

    if (semctl(sem_id, sem_num, SETVAL, arg) == -1) {
        perror("Error: Could not initialize semaphore with value\n");
        exit(EXIT_FAILURE);
    }
}


oven_and_table* save_shm_attach(int shm_id) {
    oven_and_table* ptr = shmat(shm_id, NULL, 0);
    if (ptr == (void*) -1) {
        perror("Error: Could not attach the shared memory segment\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}


int rand_range(int a, int b) {
    return random() % (b - a + 1) + a;
}


void print_stamp(pid_t pid) {
    struct timeval curr_time;
    if (gettimeofday(&curr_time, NULL) == -1) {
        perror("Error: Could not get time\n");
        exit(EXIT_FAILURE);
    }

    printf("(pid: %d; time: %ld [s] %ld [ms])", pid, curr_time.tv_sec%1000, curr_time.tv_usec/1000);
}

#endif //SYSOPY_COMMON_H
