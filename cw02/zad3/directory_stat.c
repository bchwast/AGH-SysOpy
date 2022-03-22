//
// Created by bchwast on 15/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


int statistics[7] = {0, 0, 0, 0, 0, 0, 0};


void directory_stat(char* directory_name) {
    DIR* directory = opendir(directory_name);
    if (directory == NULL) {
        printf("Cannot open directory %s\n", directory_name);
        exit(0);
    }

    struct dirent* current;
    while ((current = readdir(directory)) != NULL) {
        if (strcmp(current->d_name, ".") != 0 && strcmp(current->d_name, "..") != 0) {
            printf("%s/%s ", directory_name, current->d_name);

            char buf[10000];
            sprintf(buf, "%s/%s", directory_name, current->d_name);
            struct stat file_stat;
            if (lstat(buf, &file_stat) < 0) {
                printf("\nCannot get status of file %s\n", buf);
                return;
            }

            printf("| nlinks: %lu ", file_stat.st_nlink);

            printf("| type: ");
            if (S_ISREG(file_stat.st_mode)) {
                printf("file");
                statistics[0]++;
            }
            else if (S_ISDIR(file_stat.st_mode)) {
                printf("dir");
                statistics[1]++;
            }
            else if (S_ISCHR(file_stat.st_mode)) {
                printf("char dev");
                statistics[2]++;
            }
            else if (S_ISBLK(file_stat.st_mode)) {
                printf("block dev");
                statistics[3]++;
            }
            else if (S_ISFIFO(file_stat.st_mode)) {
                printf("fifo");
                statistics[4]++;
            }
            else if (S_ISLNK(file_stat.st_mode)) {
                printf("slink");
                statistics[5]++;
            }
            else if (S_ISSOCK(file_stat.st_mode)) {
                printf("sock");
                statistics[6]++;
            }
            printf(" ");

            printf("| size: %lu ", file_stat.st_size);

            char date[10];
            strftime(date, 10, "%d-%m-%y", localtime(&(file_stat.st_atime)));
            printf("| last access: %s ", date);
            strftime(date, 10, "%d-%m-%y", localtime(&(file_stat.st_mtime)));
            printf("| last modified: %s\n", date);
        }
        if (current->d_type == DT_DIR && strcmp(current->d_name, ".") != 0 && strcmp(current->d_name, "..") != 0) {
            char d_path[10000];
            sprintf(d_path, "%s/%s", directory_name, current->d_name);
            directory_stat(d_path);
        }
    }

    closedir(directory);
}


int main(int argc, char* argv[]) {
    char *directory_name = calloc(10000, sizeof(char));
    char *buf = calloc(10000, sizeof(char));

    if (argc > 1) {
        strcpy(directory_name, argv[1]);
    } else {
        printf("Expected parameter [directory name]\n");
        exit(0);
    }
    realpath(directory_name, buf);
    directory_stat(buf);
    free(directory_name);

    printf("Files: %d | Directories: %d | Char dev: %d | Block dev: %d | Fifo: %d | Symlinks: %d | Sockets: %d\n",
           statistics[0], statistics[1], statistics[2], statistics[3], statistics[4], statistics[5], statistics[6]);
}