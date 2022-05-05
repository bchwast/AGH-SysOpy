//
// Created by bchwast on 05/05/22.
//

#include "common.h"


const char* sem_names[NO_SEMAPHORES] = {"/IS_OVEN_USED", "/OVEN_FULL_COUNT","/IS_TABLE_USED", "/TABLE_EMPTY_COUNT", "/TABLE_FULL_COUNT"};
sem_t* sems[NO_SEMAPHORES];
int shm_fd;


sem_t* get_semaphore(int i) {
    sem_t* sem = sem_open(sem_names[i], O_RDWR);
    if (sem == SEM_FAILED) {
        perror("Error: Could not open semaphore\n");
        exit(EXIT_FAILURE);
    }
    return sem;
}


int get_pizza() {
    sem_wait(sems[TABLE_EMPTY_COUNT]);
    sem_wait(sems[IS_TABLE_USED]);

    oven_and_table* o_t = save_shm_attach(shm_fd);
    int index = o_t->taken_pizza;
    o_t->taken_pizza = (index + 1) % MAX_TABLE_SIZE;
    int pizza_type = o_t->table[index];
    o_t->table[index] = -1;
    o_t->n_pizzas_table--;
    int n_pizzas_table = o_t->n_pizzas_table;

    safe_shm_detach(o_t);
    sem_post(sems[IS_TABLE_USED]);
    sem_post(sems[TABLE_FULL_COUNT]);

    print_stamp(getpid());
    printf(" Pobieram pizze: %d. Liczba pizz na stole: %d", pizza_type, n_pizzas_table);

    return pizza_type;
}

void deliver_pizza(int pizza_type) {
    sleep(rand_range(4, 5));
    print_stamp(getpid());
    printf(" Dostarczam pizze %d\n", pizza_type);
}


int main() {
    srand(getpid());
    for (int i = 0; i < NO_SEMAPHORES; i++) {
        sems[i] = get_semaphore(i);
    }

    if ((shm_fd = shm_open(SHARED_MEMORY, O_RDWR, 0666)) == -1) {
        perror("Error: Could not open shared memory\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int pizza_type = get_pizza();
        deliver_pizza(pizza_type);
        sleep(rand_range(4, 5));
    }
}