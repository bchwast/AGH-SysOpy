//
// Created by bchwast on 05/05/22.
//

#include "common.h"


int get_pizza(int sem_id, int shm_id) {
    sem_wait(sem_id, TABLE_EMPTY_COUNT);
    sem_wait(sem_id, IS_TABLE_USED);

    oven_and_table* o_t = save_shm_attach(shm_id);
    int index = o_t->taken_pizza;
    o_t->taken_pizza = (index + 1) % MAX_TABLE_SIZE;
    int pizza_type = o_t->table[index];
    o_t->table[index] = -1;
    o_t->n_pizzas_table--;
    int n_pizzas_table = o_t->n_pizzas_table;

    shmdt(o_t);
    sem_signal(sem_id, IS_TABLE_USED);
    sem_signal(sem_id, TABLE_FULL_COUNT);

    print_stamp(getpid());
    printf(" Pobieram pizze: %d. Liczba pizz na stole: %d", pizza_type, n_pizzas_table);

    return pizza_type;
}

void deliver_pizza(int pizza_type, int sem_id, int shm_id) {
    sleep(rand_range(4, 5));
    print_stamp(getpid());
    printf(" Dostarczam pizze %d\n", pizza_type);
}


int main() {
    srand(getpid());
    int sem_id = get_semaphore();
    int shm_id = get_shared_memory();

    while (1) {
        int pizza_type = get_pizza(sem_id, shm_id);
        deliver_pizza(pizza_type, sem_id, shm_id);
        sleep(rand_range(4, 5));
    }
}