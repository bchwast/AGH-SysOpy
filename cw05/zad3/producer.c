//
// Created by bchwast on 11/04/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int n_digits(int num) {
    int sum = 0;
    while (num > 0) {
        sum++;
        num /= 10;
    }
    return sum;
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Expected 4 arguments: [fifo path] [prod no] [text file path] [N]\n");
        exit(1);
    }
    char* fifo_name = argv[1];
    char* text_name = argv[3];
    int N;
    int prod_no;
    if ((N = atoi(argv[4])) < 1 || (prod_no = atoi(argv[2])) < 1) {
        printf("Number must be positive\n");
        exit(1);
    }

    FILE* text = fopen(text_name, "r");
    if (text == NULL) {
        printf("Could not open %s\n", text_name);
        exit(1);
    }
    FILE* fifo = fopen(fifo_name, "w");
    if (fifo == NULL) {
        printf("Could not open pipe %s\n", fifo_name);
        exit(1);
    }

    int prod_no_len = n_digits(prod_no);
    int read_offset = prod_no_len + 1;

    char* buff = (char *) calloc(read_offset + N + 2, sizeof(char));
    buff[read_offset + N + 1] = 0;
    sprintf(buff, "%d|", prod_no);

    char* r_buff = buff + read_offset;

    int n_read;
    setvbuf(fifo, NULL, _IONBF, 0);

    do {
        int delay = (rand() % 15) + 5;
        usleep(delay * 1000);
        n_read = fread(r_buff, sizeof(char), N, text);

        for (size_t i = n_read; i < N; i++) {
            r_buff[i] = 0;
        }
        for (size_t i = 0; i < n_read; i++) {
            if (r_buff[i] == '\n') {
                r_buff[i] == ' ';
            }
        }
        fwrite(buff, sizeof(char), read_offset + N, fifo);
    } while (n_read > 0);

    free(buff);
    fclose(text);

    return 0;
}
