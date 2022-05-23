//
// Created by bchwast on 23/05/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define CRIT_REINDEERS 9
#define CRIT_ELVES 3
#define ALL_ELVES 10
#define MAX_GIFTS 3

int idle_reindeers_num = 0;
int waiting_elves_num = 0;
int waiting_elves_ids[CRIT_ELVES];
int let_reindeers_go = 1;

pthread_mutex_t santa_sleeping_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_spleeping_condition = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_wait_condition = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_wait_condition = PTHREAD_COND_INITIALIZER;


int rand_range(int a, int b) {
    return random() % (b - a + 1) + a;
}


void clean_threads(pthread_t* santa_thread, pthread_t** reindeer_threads, pthread_t** elves_threads, int* reindeerIDs, int* elvesIDs) {
    pthread_join(*santa_thread, NULL);
    for (int i = 0; i < CRIT_REINDEERS; i++) {
        pthread_join(*reindeer_threads[i], NULL);
    }
    for (int i = 0; i < ALL_ELVES; i++) {
        pthread_join(*elves_threads[i], NULL);
    }

    free(reindeerIDs);
    free(reindeer_threads);
    free(elvesIDs);
    free(elves_threads);
}


void* santa(void* args) {
    srand(time(NULL));
    int delivered_gifts = 0;

    while (delivered_gifts < MAX_GIFTS) {
        pthread_mutex_lock(&santa_sleeping_mutex);
        while (idle_reindeers_num < CRIT_REINDEERS && waiting_elves_num < CRIT_ELVES) {
            printf("Mikołaj: śpię\n");
            pthread_cond_wait(&santa_spleeping_condition, &santa_sleeping_mutex);
        }
        pthread_mutex_unlock(&santa_sleeping_mutex);

        printf("Mikołaj: budzę się\n");

        pthread_mutex_lock(&reindeer_mutex);
        if (idle_reindeers_num == CRIT_REINDEERS) {
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(rand_range(2, 4));
            delivered_gifts++;
            idle_reindeers_num = 0;

            pthread_mutex_lock(&reindeer_wait_mutex);
            let_reindeers_go = 1;
            pthread_cond_broadcast(&reindeer_wait_condition);
            pthread_mutex_unlock(&reindeer_wait_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_lock(&elf_mutex);
        if (waiting_elves_num == CRIT_ELVES) {
            pthread_mutex_lock(&elf_wait_mutex);
            printf("Mikołaj: rozwiązuję problemy elfów ");
            for (int i = 0; i < CRIT_ELVES; i++) {
                printf("%d ", waiting_elves_ids[i]);
                waiting_elves_ids[i] = -1;
            }
            printf("\n");

            sleep(rand_range(1, 2));
            waiting_elves_num = 0;
            pthread_cond_broadcast(&elf_wait_condition);
            pthread_mutex_unlock(&elf_wait_mutex);
        }
        pthread_mutex_unlock(&elf_mutex);

        printf("Mikołaj: zasypiam\n");
    }
    exit(EXIT_SUCCESS);
}


void* reindeer(void* arg) {
    int ID = *((int *) arg);
    srand(ID);

    while (1) {
        pthread_mutex_lock(&reindeer_wait_mutex);
        while (!let_reindeers_go) {
            pthread_cond_wait(&reindeer_wait_condition, &reindeer_wait_mutex);
        }
        pthread_mutex_unlock(&reindeer_wait_mutex);

        sleep(rand_range(5, 10));

        pthread_mutex_lock(&reindeer_mutex);
        idle_reindeers_num++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", idle_reindeers_num, ID);
        let_reindeers_go = 0;

        if (idle_reindeers_num == CRIT_REINDEERS) {
            printf("Refinfer: wybudzam Mikołaja, %d\n", ID);
            pthread_mutex_lock(&santa_sleeping_mutex);
            pthread_cond_broadcast(&santa_spleeping_condition);
            pthread_mutex_unlock(&santa_sleeping_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        sleep(rand_range(2, 4));
    }
}


void* elf(void* arg) {
    int ID = *((int *) arg);
    srand(ID);

    while (1) {
        sleep(rand_range(2, 5));

        pthread_mutex_lock(&elf_wait_mutex);
        while (waiting_elves_num == CRIT_ELVES) {
            printf("Elf: elf czeka na powrót elfów, %d\n", ID);
            pthread_cond_wait(&elf_wait_condition, &elf_wait_mutex);
        }
        pthread_mutex_unlock(&elf_wait_mutex);

        pthread_mutex_lock(&elf_mutex);
        if (waiting_elves_num < CRIT_ELVES) {
            waiting_elves_ids[waiting_elves_num] = ID;
            waiting_elves_num++;
            printf("Elf: czeka %d elfów na Mikołaja, %d\n", waiting_elves_num, ID);

            if (waiting_elves_num == CRIT_ELVES) {
                printf("Elf: wybudzam Mikołaja, %d\n", ID);
                pthread_mutex_lock(&santa_sleeping_mutex);
                pthread_cond_broadcast(&santa_spleeping_condition);
                pthread_mutex_unlock(&santa_sleeping_mutex);
            }
        }
        pthread_mutex_unlock(&elf_mutex);
    }
}


int main() {
    for (int i = 0; i < CRIT_ELVES; i++) {
        waiting_elves_ids[i] = -1;
    }

    int* reindeerIDs = calloc(CRIT_REINDEERS, sizeof(int));
    pthread_t* reindeer_threads = calloc(CRIT_REINDEERS, sizeof(pthread_t));

    int* elvesIDs = calloc(ALL_ELVES, sizeof(int));
    pthread_t* elves_threads = calloc(ALL_ELVES, sizeof(pthread_t));

    pthread_t santa_thread;
    if (pthread_create(&santa_thread, NULL, *santa, NULL) != 0) {
        clean_threads(&santa_thread, &reindeer_threads, &elves_threads, reindeerIDs, elvesIDs);
        perror("Error: Could not create santa thread\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < CRIT_REINDEERS; i++) {
        reindeerIDs[i] = i;
        if (pthread_create(&reindeer_threads[i], NULL, &reindeer, &reindeerIDs[i]) != 0) {
            clean_threads(&santa_thread, &reindeer_threads, &elves_threads, reindeerIDs, elvesIDs);
            perror("Error: Could not create reindeer thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < ALL_ELVES; i++) {
        elvesIDs[i] = i;
        if (pthread_create(&elves_threads[i], NULL, &elf, &elvesIDs[i]) != 0) {
            clean_threads(&santa_thread, &reindeer_threads, &elves_threads, reindeerIDs, elvesIDs);
            perror("Error: Could not create elf thread\n");
            exit(EXIT_FAILURE);
        }
    }

    clean_threads(&santa_thread, &reindeer_threads, &elves_threads, reindeerIDs, elvesIDs);
    return 0;
}