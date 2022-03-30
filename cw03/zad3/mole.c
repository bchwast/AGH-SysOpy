//
// Created by bchwast on 25/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <dirent.h>
#include <string.h>


int contains(char* filename, char* sample) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        exit(1);
    }

    char* line = calloc(100000, sizeof(char));
    while (fgets(line, 100000 * sizeof(char), file)) {
        if (strstr(line, sample)) {
            return 1;
        }
    }
    return 0;
}


void browse_directory(char* directory_name, char* sample, int depth) {
    if (depth == 0) {
        exit(0);
    }
    DIR* directory = opendir(directory_name);
    if (directory == NULL) {
        printf("Cannot open directory %s\n", directory_name);
        exit(1);
    }

    char* extension;
    struct dirent* current;
    while ((current = readdir(directory))) {
        if ((strcmp(current->d_name, ".") == 0) || (strcmp(current->d_name, "..") == 0)) {
            continue;
        }
        char* path = calloc(strlen(directory_name) + 2 + strlen(current->d_name), sizeof(char));
        sprintf(path, "%s/%s", directory_name, current->d_name);

        if (current->d_type == DT_DIR && fork() == 0) {
            browse_directory(path, sample, depth - 1);
            exit(0);
        }

        extension = strrchr(current->d_name, '.');

        if (current->d_type == DT_REG && extension != NULL && strcmp(extension, ".txt") == 0) {
            if (contains(path, sample)) {
                printf("%s\t%d\tfound\n", path, (int) getpid());
            }
        }

        wait(NULL);
        free(path);
    }

    closedir(directory);
}



int main(int argc, char* argv[]) {
    char* directory;
    char* sample;
    int depth;

    if (argc < 4) {
        printf("Expected parameters: [directory] [desired string] [depth]\n");
        exit(0);
    }
    else {
        directory = argv[1];
        sample = argv[2];
        depth = atoi(argv[3]);
    }

    browse_directory(directory, sample, depth);

    return 0;
}