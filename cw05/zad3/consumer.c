//
// Created by bchwast on 11/04/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>


void add_to_text(FILE* text, char* buff, int coming) {
    int text_desc = fileno(text);
    flock(text_desc, LOCK_EX);
    size_t n_lines = 1;
    char c;
    fseek(text, 0, SEEK_SET);
    int before = -1;

    while (fread(&c, sizeof(char), 1, text) > 0) {
        if ((c == '\n' || c == 0x0) && coming == n_lines) {
            before = ftell(text) - 1;
        }
        if (c == '\n') {
            n_lines++;
        }
    }

    size_t file_size = ftell(text);
    if (before != -1) {
        fseek(text, before, SEEK_SET);
        char* after = calloc(file_size - before, sizeof(char));
        fread(after, sizeof(char), file_size - before, text);
        fseek(text, before, SEEK_SET);
        fwrite(buff, sizeof(char), strlen(buff), text);
        fwrite(after, sizeof(char), file_size - before, text);
        free(after);
    } else {
        while (coming != n_lines) {
            fwrite("\n", sizeof(char), 1, text);
            n_lines++;
        }
        fwrite(buff, sizeof(char), strlen(buff), text);
    }
    fseek(text, 0, SEEK_SET);
    flock(text_desc, LOCK_UN);
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Expected 3 arguments: [fifo path] [text file path] [N]\n");
        exit(1);
    }
    char* fifo_name = argv[1];
    char* text_name = argv[2];
    int N;
    if ((N = atoi(argv[3])) < 1) {
        printf("Number must be positive\n");
        exit(1);
    }

    FILE* fifo = fopen(argv[1], "r");
    if (fifo == NULL) {
        printf("Could not open fifo\n");
        exit(1);
    }
    FILE* text = fopen(argv[2], "w+r");
    if (text == NULL) {
        printf("Could not open text file\n");
        exit(1);
    }

    setvbuf(fifo, NULL, _IONBF, 0);
    int fifo_desc = fileno(fifo);
    int coming;
    int res;

    while (flock(fifo_desc, LOCK_EX) == 0 && (res = fscanf(fifo, "%d|", &coming)) >= 0) {
        if (res == 0) {
            flock(fifo_desc, LOCK_UN);
            continue;
        }

        char buff[N + 1];
        buff[N] = 0;
        size_t read_len = 0;
        while (read_len < N) {
            read_len += fread(buff + read_len, sizeof(char), N - read_len, fifo);
        }
        buff[read_len] = 0;
        flock(fifo_desc, LOCK_UN);
        add_to_text(text, buff, coming);
    }

    return 0;
}

