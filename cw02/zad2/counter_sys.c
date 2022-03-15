//
// Created by bchwast on 15/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <fcntl.h>
#define BUFFER_SIZE 256


long double time_sec(clock_t time){
    return (long double)(time) / sysconf(_SC_CLK_TCK);
}


void print_res(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms, FILE* file){
    fprintf(file, "EXECUTION TIME\n");
    fprintf(file, "real %Lf\n", time_sec(clock_end - clock_start));
    fprintf(file, "user %Lf\n", time_sec(end_tms.tms_utime - start_tms.tms_utime));
    fprintf(file, "sys  %Lf\n", time_sec(end_tms.tms_stime - start_tms.tms_stime));
}


void find_char_sys(char sample, char* file_name) {
    int file = open(file_name, O_RDONLY);
    if (file < 0) {
        printf("Cannot open file %s\n", file_name);
        exit(0);
    }

    char* line = calloc(BUFFER_SIZE, sizeof(char));
    int cnt_chars = 0;
    int cnt_lines = 0;
    int file_read = 0;
    long int nread;
    int i = 0;

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
            int found = 0;
            for (int ind = 0; ind < i; ind++) {
                if (line[ind] == sample) {
                    cnt_chars++;
                    if (!found) {
                        found = 1;
                        cnt_lines++;
                    }
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
    printf("Character %c has appeared %d times in %d lines in file %s\n", sample, cnt_chars, cnt_lines, file_name);
}


int main(int argc, char* argv[]) {
    char sample;
    char* file_name = calloc(10000, sizeof(char));

    if (argc > 2) {
        sample = *(argv[1]);
        strcpy(file_name, argv[2]);
    }
    else {
        printf("Expected parameters: [character] [file name]\n");
        exit(0);
    }

    FILE* result_file = fopen("pomiar_zad_2_sys.txt", "w");
    struct tms start_tms;
    struct tms end_tms;
    clock_t clock_start;
    clock_t clock_end;

    clock_start = times(&start_tms);
    find_char_sys(sample, file_name);
    clock_end = times(&end_tms);
    print_res(clock_start, clock_end, start_tms, end_tms,result_file);

    free(file_name);
}