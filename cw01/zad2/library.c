//
// Created by bchwast on 03/03/2022.
//

#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char tmp_file[] = "tmp.txt";
char** array;
unsigned int size = 0;

void create_array(unsigned int size_) {
    if (array != NULL) {
        printf("The array has already been allocated\n\n");
        return;
    }

    array = calloc(size_, sizeof(char *));
    size = size_;
}

void execute_wc(char* files) {
    char cmd[10240];
    char cmd_beg[10240] = "wc ";
    char cmd_end[] = ">> tmp.txt";

    int ret = snprintf(cmd, sizeof(cmd), "%s%s%s", cmd_beg, files, cmd_end);
    if (ret < 0) {
        printf("Combined length of file names is longer than 1000 characters\n\n");
        exit(0);
    }

    system(cmd);
}

int insert(char* block) {
    int inserted = 0;

    if (array == NULL) {
        printf("The array has not been allocated yet\n\n");
        exit(0);
    }

    int ind = 0;
    while (ind < size) {
        if (array[ind] == NULL) {
            array[ind] = block;
            inserted = 1;
            break;
        }

        ind++;
    }

    if (!inserted) {
        printf("The array is too small\n\n");
        exit(0);
    }

    return ind;
}

char* allocate_block(void) {
    char* block;
    FILE* file = fopen(tmp_file, "r");

    if (file == NULL) {
        perror("Cannot open file");
        exit(1);
    }

    if (fseek(file, 0L, SEEK_END) == 0) {
        long long file_len = ftell(file);
        if (file_len == -1) {
            printf("Couldn't read the size of tmp.txt\n\n");
            exit(0);
        }

        block = calloc(file_len, sizeof(char) + 1);

        if (fseek(file, 0L, SEEK_SET) != 0) {
            printf("Couldn't rewind tmp.txt");
            exit(0);
        }

        size_t new_len = fread(block, sizeof(char), file_len, file);
        if (ferror(file) != 0) {
            printf("Error reading tmp.txt");
            exit(0);
        }
        else {
            block[new_len++] = '\0';
        }
    }

    fclose(file);
    remove(tmp_file);
    return block;
}

int wc_files(char* files) {
    execute_wc(files);
    char* block = allocate_block();
    int index = insert(block);
    return index;
}

void remove_block(int index) {
    if (array == NULL) {
        printf("The array has not been allocated yet\n\n");
        exit(0);
    }

    if (size <= index) {
        printf("The array is smaller than %d\n\n", index);
        exit(0);
    }

    if (array[index] == NULL) {
        printf("There is no block under the %d index\n\n", index);
        exit(0);
    }

    free(array[index]);

    for (int i = index; i < size; i++) {
        array[i] = array[i + 1];
    }
}

void print_array() {
    for (int i = 0; i < size; i++) {
        if (array[i] == NULL) {
            printf("NULL\n");
        }
        else {
            printf("%s\n", array[i]);
        }
    }
}

void remove_array() {
    if (array == NULL) {
        printf("The array has not been allocated yet\n\n");
        exit(0);
    }

    for (int i = 0; i < size; i++) {
        free(array[i]);
    }

    free(array);
    array = NULL;

    size = 0;
}