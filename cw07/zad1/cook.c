//
// Created by bchwast on 05/05/22.
//

#include "common.h"


int prepare_pizza() {
    int pizza_type = rand_range(0, 9);
    sleep(rand_range(1, 2));
    print_stamp(getpid());
    printf(" Przygotowuje pizze: %d\n", pizza_type);
    fflush(stdout);

    return pizza_type;
}


void bake_pizza(int pizza_type, int sem_id, int shm_id) {
    sem_wait(sem_id, OVEN_FULL_COUNT);
    sem_wait(sem_id, IS_OVEN_USED);

    oven_and_table* o_t = save_shm_attach(shm_id);
    int index = (o_t->last_pizza + o_t->n_pizzas_oven) % MAX_OVEN_SIZE;
    o_t->oven[index] = pizza_type;

    o_t->n_pizzas_oven++;
    int n_pizzas_oven = o_t->n_pizzas_oven;
    shmdt(o_t);

    sem_signal(sem_id, IS_OVEN_USED);

    print_stamp(getpid());
    printf(" Dodalem pizze: %d. Liczba pizz w piecu: %d\n", pizza_type, n_pizzas_oven);
    fflush(stdout);
}


void put_pizza_on_table(int sem_id, int shm_id) {
    sem_wait(sem_id, IS_OVEN_USED);
    oven_and_table* o_t = save_shm_attach(shm_id);

    int index = o_t->last_pizza;
    int pizza_type = o_t->oven[index];
    o_t->oven[index] = -1;
    o_t->n_pizzas_oven--;
    o_t->last_pizza = (index + 1) % MAX_OVEN_SIZE;
    int n_pizzas_oven = o_t->n_pizzas_oven;

    shmdt(o_t);

    sem_signal(sem_id, IS_OVEN_USED);
    sem_signal(sem_id, OVEN_FULL_COUNT);

    sem_wait(sem_id, TABLE_FULL_COUNT);
    sem_wait(sem_id, IS_TABLE_USED);
    o_t = save_shm_attach(shm_id);

    int tab_index = (o_t->taken_pizza + o_t->n_pizzas_table) % MAX_TABLE_SIZE;
    o_t->table[tab_index] = pizza_type;
    o_t->n_pizzas_table++;
    int n_pizzas_table = o_t->n_pizzas_table;

    shmdt(o_t);
    sem_signal(sem_id, IS_TABLE_USED);
    sem_signal(sem_id, TABLE_EMPTY_COUNT);

    print_stamp(getpid());
    printf(" Wyjmuje pizze: %d. Liczba pizz w piecu: %d, Liczba pizz na stole: %d\n", pizza_type, n_pizzas_oven, n_pizzas_table);
}


int main() {
    srand(getpid());
    int sem_id = get_semaphore();
    int shm_id = get_shared_memory();

    while (1) {
        int pizza = prepare_pizza();
        bake_pizza(pizza, sem_id, shm_id);
        sleep(rand_range(4, 5));
        put_pizza_on_table(sem_id, shm_id);
    }
}