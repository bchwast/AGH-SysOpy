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
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <wait.h>


#define MAX_DELIVERERS 30
#define MAX_COOKS 30
#define MAX_OVEN_SIZE 5
#define MAX_TABLE_SIZE 5

#define IS_OVEN_USED 0
#define OVEN_FULL_COUNT 1
#define IS_TABLE_USED 2
#define TABLE_EMPTY_COUNT 3
#define TABLE_FULL_COUNT 4

#define NO_SEMAPHORES 5
#define SHARED_MEMORY "/SHARED_MEMORY"


typedef struct oven_and_table {
    int oven[MAX_OVEN_SIZE];
    int table[MAX_TABLE_SIZE];

    int last_pizza;
    int n_pizzas_oven;

    int taken_pizza;
    int n_pizzas_table;
} oven_and_table;


char* get_home_path() {
    char* path = getenv("HOME");
    if (path == NULL) {
        path = getpwuid(getuid())->pw_dir;
    }
    return path;
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


oven_and_table* save_shm_attach(int shm_fd) {
    oven_and_table* ptr = mmap(NULL, sizeof(oven_and_table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == (void*) -1) {
        perror("Error: Could not attach the shared memory segment\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}


void safe_shm_detach(oven_and_table* o_t) {
    if (munmap(o_t, sizeof(oven_and_table)) == -1) {
        perror("Error: Could not close shared memory\n");
        exit(EXIT_FAILURE);
    }
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
