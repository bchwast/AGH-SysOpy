//
// Created by bchwast on 15/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        if (chr != ' ' && chr != '\0' && chr != '\n' && chr != '\t' && chr != '\r') {
            return 0;
        }
    } while (chr != '\0');

    return 1;
}


void copy_lib(char* file_name, char* output_name) {
    FILE* file = fopen(file_name, "r");
    FILE* output = fopen(output_name, "w");
    if (file == NULL) {
        printf("Cannot open file %s\n", file_name);
        exit(0);
    }
    if (output == NULL) {
        printf("Cannot open file %s\n", output_name);
        exit(0);
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file) !=  NULL) {
        if (!is_empty(line)) {
            fputs(line, output);
        }
    }

    fclose(file);
    fclose(output);
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

    FILE* result_file = fopen("pomiar_zad_1_lib.txt", "w");
    struct tms start_tms;
    struct tms end_tms;
    clock_t clock_start;
    clock_t clock_end;

    clock_start = times(&start_tms);
    copy_lib(file_name, output_name);
    clock_end = times(&end_tms);
    print_res(clock_start, clock_end, start_tms, end_tms,result_file);

    free(file_name);
    free(output_name);
}