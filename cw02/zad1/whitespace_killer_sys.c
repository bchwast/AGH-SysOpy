//
// Created by bchwast on 15/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>
#define BUFFER_SIZE 10000


long double time_sec(clock_t time){
    return (long double)(time) / sysconf(_SC_CLK_TCK);
}


void print_res(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms, FILE* file){
    fprintf(file, "EXECUTION TIME\n");
    fprintf(file, "real %Lf\n", time_sec(clock_end - clock_start));
    fprintf(file, "user %Lf\n", time_sec(end_tms.tms_utime - start_tms.tms_utime));
    fprintf(file, "sys  %Lf\n", time_sec(end_tms.tms_stime - start_tms.tms_stime));
}


int is_empty(char* str) {
    char chr;
    do {
        chr = *(str++);
        if (chr != ' ' && chr != '\0' && chr != '\n' && chr != '\t' && chr != '\r' && chr != '\v' && chr != '\f') {
            return 0;
        }
    } while (chr != '\0' && chr != '\n');

    return 1;
}


void copy_sys(char* file_name, char* output_name) {
    int file = open(file_name, O_RDONLY);
    int output = open(output_name, O_CREAT | O_WRONLY | O_TRUNC);
    if (file < 0) {
        printf("Cannot open file %s\n", file_name);
        exit(0);
    }
    if (output < 0) {
        printf("Cannot open file %s\n", output_name);
        exit(0);
    }

    long int nread;
    int file_read = 0;
    int i = 0;
    char* line = calloc(BUFFER_SIZE, sizeof(char));
    while (file_read == 0) {
        nread = read(file, &line[i], 1);
        if (nread == -1) {
            printf("Error with reading\n");
            exit(0);
        }
        if (nread == 0) {
            file_read = 1;
        }

        if (line[i] == '\n' || line[i] == '\0') {
            if (!is_empty(line)) {
                if (line[i] == '\0') {
                    i--;
                }
                if (write(output, line, i + 1) != i + 1) {
                    printf("Error with writing\n");
                    exit(0);
                }
            }
            free(line);
            line = calloc(BUFFER_SIZE, sizeof(char));
            i = 0;
        }
        else {
            i++;
        }
    }
    free(line);
    close(file);
    close(output);
}


int main(int argc, char* argv[]) {
    char* file_name = calloc(BUFFER_SIZE, sizeof(char));
    char* output_name = calloc(BUFFER_SIZE, sizeof(char));

    if (argc > 2) {
        strcpy(file_name, argv[1]);
        strcpy(output_name, argv[2]);
    }
    else {
        printf("Enter source file name:\n");
        scanf("%s", file_name);
        printf("\nEnter output file name:\n");
        scanf("%s", output_name);
    }

    FILE* result_file = fopen("pomiar_zad_1_sys.txt", "w");
    struct tms start_tms;
    struct tms end_tms;
    clock_t clock_start;
    clock_t clock_end;

    clock_start = times(&start_tms);
    copy_sys(file_name, output_name);
    clock_end = times(&end_tms);
    print_res(clock_start, clock_end, start_tms, end_tms,result_file);

    free(file_name);
    free(output_name);
}