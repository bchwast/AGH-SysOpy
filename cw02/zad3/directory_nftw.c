//
// Created by bchwast on 15/03/2022.
//

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>


int statistics[7] = {0, 0, 0, 0, 0, 0, 0};


int stats(const char *file_path, const struct stat *file_stat, int tflag, struct FTW *s) {
    printf("%s ", file_path);

    printf("| nlinks: %lu ", file_stat->st_nlink);

    printf("| type: ");
    if (S_ISREG(file_stat->st_mode)) {
        printf("file");
        statistics[0]++;
    }
    else if (S_ISDIR(file_stat->st_mode)) {
        printf("dir");
        statistics[1]++;
    }
    else if (S_ISCHR(file_stat->st_mode)) {
        printf("char dev");
        statistics[2]++;
    }
    else if (S_ISBLK(file_stat->st_mode)) {
        printf("block dev");
        statistics[3]++;
    }
    else if (S_ISFIFO(file_stat->st_mode)) {
        printf("fifo");
        statistics[4]++;
    }
    else if (S_ISLNK(file_stat->st_mode)) {
        printf("slink");
        statistics[5]++;
    }
    else if (S_ISSOCK(file_stat->st_mode)) {
        printf("sock");
        statistics[6]++;
    }
    printf(" ");

    printf("| size: %lu ", file_stat->st_size);

    char date[10];
    strftime(date, 10, "%d-%m-%y", localtime(&(file_stat->st_atime)));
    printf("| last access: %s ", date);
    strftime(date, 10, "%d-%m-%y", localtime(&(file_stat->st_mtime)));
    printf("| last modified: %s\n", date);

    return 0;
};


int main(int argc, char* argv[]) {
    char *directory_name = calloc(10000, sizeof(char));
    char *buf = calloc(10000, sizeof(char));
    int flags = 0;

    if (argc > 1) {
        strcpy(directory_name, argv[1]);
    } else {
        printf("Expected parameter [directory name]\n");
        exit(0);
    }
    realpath(directory_name, buf);

    if (nftw(buf, stats, 20, flags) == -1) {
        printf("Error with nftw\n");
        exit(0);
    }

    free(directory_name);

    printf("Files: %d | Directories: %d | Char dev: %d | Block dev: %d | Fifo: %d | Symlinks: %d | Sockets: %d\n",
           statistics[0], statistics[1], statistics[2], statistics[3], statistics[4], statistics[5], statistics[6]);
}