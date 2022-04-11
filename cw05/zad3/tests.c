//
// Created by bchwast on 11/04/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>


const char* inputs[10];


int test_output(FILE* text, int lines) {
    if (lines == 1) {
        char* buff = (char *) calloc(2000, sizeof(char));
        fgets(buff, sizeof(char) * 2000, text);
        if (strcmp(buff, "mmmmm") != 0) {
            free(buff);
            return 0;
        }
        free(buff);
        buff = (char *) calloc(2000, sizeof(char));
        if (fgets(buff, sizeof(char) * 2000, text) != NULL) {
            free(buff);
            return 0;
        }
        free(buff);
        return 1;
    } else {
        char* buff = (char *) calloc(2000, sizeof(char));
        fgets(buff, sizeof(char) * 2000, text);
        if (strcmp(buff, "mmmmm\n") != 0) {
            free(buff);
            return 0;
        }
        free(buff);
        buff = (char *) calloc(2000, sizeof(char));
        fgets(buff, sizeof(char) * 2000, text);
        if (strcmp(buff, "aaaaa\n") != 0) {
            free(buff);
            return 0;
        }
        free(buff);
        buff = (char *) calloc(2000, sizeof(char));
        fgets(buff, sizeof(char) * 2000, text);
        if (strcmp(buff, "22222\n") != 0) {
            free(buff);
            return 0;
        }
        free(buff);
        buff = (char *) calloc(2000, sizeof(char));
        fgets(buff, sizeof(char) * 2000, text);
        if (strcmp(buff, "11111\n") != 0) {
            return 0;
        }
        free(buff);
        buff = (char *) calloc(2000, sizeof(char));
        fgets(buff, sizeof(char) * 2000, text);
        if (strcmp(buff, "kkkkk") != 0) {
            free(buff);
            return 0;
        }
        free(buff);
        buff = (char *) calloc(2000, sizeof(char));
        if (fgets(buff, sizeof(char) * 2000, text) != NULL) {
            free(buff);
            return 0;
        }
        free(buff);
        return 1;
    }
}


char* int_to_str(int val) {
    static char buff[6];
    sprintf(buff, "%i", val);
    return strdup(buff);
}


int test(int no_of_producers, int no_of_consumers, int N) {
    char* N_str = int_to_str(N);

    for (int i = 0; i < no_of_producers; i++) {
        char* i_str = int_to_str(i + 1);
        if (fork() == 0) {
            execl("./producer", "./producer", "./mazik", i_str, inputs[i], N_str, NULL);
        }
        free(i_str);
    }

    for (int i = 0; i < no_of_consumers; i++) {
        if (fork() == 0) {
            execl("./consumer", "./consumer", "./mazik", "cons/maz.txt", N_str, NULL);
        }
    }

    free(N_str);
    while (wait(NULL) != -1);

    FILE* text = fopen("cons/maz.txt", "r");
    if (text == NULL) {
        printf("Could not open maz.txt\n");
        exit(1);
    }

    if (test_output(text, no_of_producers)) {
        return 1;
    } else {
        return 0;
    }
}


int main() {
    if (mkfifo("mazik", 0666) < 0) {
        printf("Could not create fifo\n");
        exit(1);
    }

    inputs[0] = "prod/m.txt";
    inputs[1] = "prod/a.txt";
    inputs[2] = "prod/2.txt";
    inputs[3] = "prod/1.txt";
    inputs[4] = "prod/k.txt";

    int no_of_lines[] = {5, 100, 5000};
    int no_of_producers[] = {1, 5, 1, 5};
    int no_of_customers[] = {1, 1, 5, 5};

    for (int i = 0; i < 3; i++) {
        printf("N = %d\n", no_of_lines[i]);
        for (int j = 0; j < 4; j++) {
            printf("Producents = %d\nCustomers = %d\n", no_of_producers[j], no_of_customers[j]);
            if (test(no_of_producers[j], no_of_customers[j], no_of_lines[i])) {
                printf("Success\n");
            } else {
                printf("Failure\n");
            }
        }
    }

    system("rm mazik");

    return 0;
}