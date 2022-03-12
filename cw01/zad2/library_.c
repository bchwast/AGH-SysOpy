//
// Created by bchwast on 12/03/2022.
//

#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Array* create_array(int size) {
    struct Array* array = calloc(1, sizeof(struct Array));

    array->size = size;
    array->last_index = -1;
    array->blocks = calloc(size, sizeof(struct Block));
    printf("Created array of size %d\n\n", size);

    return array;
}

void wc_files(char* files) {
//    va_list valist;
//    va_start(valist, amm);

    char cmd[1024];
    char cmd_beg[1024] = "wc ";
    char cmd_end[] = ">> tmp.txt";

    int ret = snprintf(cmd, sizeof(cmd), "%s%s%s", cmd_beg, files, cmd_end);
    if (ret < 0) {
        abort();
    }

//    snprintf(cmd, sizeof(cmd), "%s", cmd_beg);
//    snprintf(cmd_beg, sizeof(cmd_beg), "%s", cmd);
//
//    for (int i = 0; i < amm; i++) {
//        char* file_name = va_arg(valist, char*);
//        FILE* file = fopen(file_name, "r");
//
//        if (file == NULL) {
//            perror("Cannot open file");
//            exit(1);
//        }
//
//        int ret = snprintf(cmd, sizeof(cmd), "%s%s%s", cmd_beg, file_name, " ");
//        if (ret < 0) {
//            abort();
//        }
//        ret = snprintf(cmd_beg, sizeof(cmd_beg), "%s", cmd);
//        if (ret < 0) {
//            abort();
//        }
//    }
//
//    int ret = snprintf(cmd, sizeof(cmd), "%s%s", cmd_beg, cmd_end);
//    if (ret < 0) {
//        abort();
//    }
    system(cmd);
}

int allocate(char* file_name, struct Array* array) {
    FILE* file = fopen(file_name, "r");

    if (file == NULL) {
        perror("Cannot open file");
        exit(1);
    }

    struct Block* block = calloc(1, sizeof(struct Block));

    fseek(file, 0L, SEEK_END);
    int file_len = ftell(file);
    fseek(file, 0L, SEEK_SET);
    block->result = (char **) calloc(file_len + 1, sizeof(char *));

    int indx = 0;
    char* temp = (char *) calloc(256, sizeof(char));
    while (fgets(temp, 256 * sizeof(char), file)) {
        char* line = (char *) calloc(256, sizeof(char));
        strcpy(line, temp);
        block->result[indx++] = line;
    }

    fclose(file);
    remove(file_name);

    array->blocks[array->last_index + 1] = block;
    array->last_index++;
    return array->last_index;
}

int manage_files(struct Array* array, char* files) {
    wc_files(files);
    return allocate("tmp.txt", array);
}

void remove_block(struct Array* array, int index) {
    if (array->blocks[index] == NULL) return;

    free(array->blocks[index]);
    array->blocks[index] = NULL;

    for (int i = index; i < array->size - 1; i++) {
        array->blocks[i] = array->blocks[i + 1];
    }

    printf("Removed block at index %d\n\n", index);
}

