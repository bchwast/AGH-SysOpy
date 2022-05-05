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


int prepare_pizza() {
    int pizza_type = rand_range(0, 9);
    sleep(rand_range(1, 2));
    print_stamp(getpid());
    printf(" Przygotowuje pizze: %d\n", pizza_type);
    fflush(stdout);

    return pizza_type;
}


void bake_pizza(int pizza_type) {
    sem_wait(sems[OVEN_FULL_COUNT]);
    sem_wait(sems[IS_OVEN_USED]);

    oven_and_table* o_t = save_shm_attach(shm_fd);
    int index = (o_t->last_pizza + o_t->n_pizzas_oven) % MAX_OVEN_SIZE;
    o_t->oven[index] = pizza_type;

    o_t->n_pizzas_oven++;
    int n_pizzas_oven = o_t->n_pizzas_oven;

    safe_shm_detach(o_t);
    sem_post(sems[IS_OVEN_USED]);

    print_stamp(getpid());
    printf(" Dodalem pizze: %d. Liczba pizz w piecu: %d\n", pizza_type, n_pizzas_oven);
    fflush(stdout);
}


void put_pizza_on_table() {
    sem_wait(sems[IS_OVEN_USED]);
    oven_and_table* o_t = save_shm_attach(shm_fd);

    int index = o_t->last_pizza;
    int pizza_type = o_t->oven[index];
    o_t->oven[index] = -1;
    o_t->n_pizzas_oven--;
    o_t->last_pizza = (index + 1) % MAX_OVEN_SIZE;
    int n_pizzas_oven = o_t->n_pizzas_oven;

    safe_shm_detach(o_t);
    if (pizza_type == -1) {
        perror("Error: Wrong type of pizza taken\n");
        exit(EXIT_FAILURE);
    }

    sem_post(sems[IS_OVEN_USED]);
    sem_post(sems[OVEN_FULL_COUNT]);

    sem_wait(sems[TABLE_FULL_COUNT]);
    sem_wait(sems[IS_TABLE_USED]);
    o_t = save_shm_attach(shm_fd);

    int tab_index = (o_t->taken_pizza + o_t->n_pizzas_table) % MAX_TABLE_SIZE;
    o_t->table[tab_index] = pizza_type;
    o_t->n_pizzas_table++;
    int n_pizzas_table = o_t->n_pizzas_table;

    safe_shm_detach(o_t);
    sem_post(sems[IS_TABLE_USED]);
    sem_post(sems[TABLE_EMPTY_COUNT]);

    print_stamp(getpid());
    printf(" Wyjmuje pizze: %d. Liczba pizz w piecu: %d, Liczba pizz na stole: %d\n", pizza_type, n_pizzas_oven, n_pizzas_table);
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
        int pizza = prepare_pizza();
        bake_pizza(pizza);
        sleep(rand_range(4, 5));
        put_pizza_on_table();
    }
}