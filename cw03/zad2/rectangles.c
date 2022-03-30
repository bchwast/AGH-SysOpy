//
// Created by bchwast on 24/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#ifdef TEST
    #include <sys/times.h>


    clock_t start_time, end_time;
    struct tms start_tms, end_tms;

    void start_measurement(){
        start_time = times(&start_tms);
    }

    void end_and_print_measurement(char *name){
        end_time = times(&end_tms);
        int ticks_per_sec = sysconf(_SC_CLK_TCK);

        double real_time = (double)(end_time - start_time) / ticks_per_sec;
        double user_time = (double)(end_tms.tms_utime - start_tms.tms_utime) / ticks_per_sec;
        double sys_time = (double)(end_tms.tms_stime - start_tms.tms_stime) / ticks_per_sec;
        printf("%35s:\t%20f\t%20f\t%20f\n",
               name, real_time, user_time, sys_time);
    }

    void print_header(){
        printf("%35s:\t%20s\t%20s\t%20s\n",
               "name", "real_time [s]", "user_time [s]", "sys_time [s]");
    }
#endif


void integrate(double width, int n) {
    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            double area = 0.0;

            for (double x = (double) i / n; x < (double) (i + 1) / n && x < 1; x += width) {
                area += (4 * width) / (x * x + 1);
            }
            char* filename = calloc(20, sizeof(char));
            sprintf(filename, "w%d.txt", i + 1);
            FILE* file = fopen(filename, "w");
            fprintf(file, "%f", area);
            fclose(file);
            free(filename);
            exit(0);
        }
    }

    for (int i = 0; i < n; i++) {
        wait(0);
    }
}


int main(int argc, char* argv[]) {
    double width;
    int n;

    if (argc > 2) {
        width = atof(argv[1]);
        n = atoi(argv[2]);
    }
    else {
        printf("You need to specify the width and number of subprocesses\n");
        exit(1);
    }

#ifdef TEST
    start_time = times(&start_tms);
#endif

    integrate(width, n);

    double area;
    double res = 0.0;

    for (int i = 0; i < n; i++) {
        char* filename = calloc(20, sizeof(char));
        sprintf(filename, "w%d.txt", i + 1);
        FILE* file = fopen(filename, "r");
        fscanf(file, "%lf", &area);
        fclose(file);
        free(filename);

        res += area;
    }

    printf("%f\n", res);

#ifdef TEST
    end_time = times(&end_tms);
    int ticks_per_sec = sysconf(_SC_CLK_TCK);

    double real_time = (double)(end_time - start_time) / ticks_per_sec;
    double user_time = (double)(end_tms.tms_utime - start_tms.tms_utime) / ticks_per_sec;
    double sys_time = (double)(end_tms.tms_stime - start_tms.tms_stime) / ticks_per_sec;
    printf("%20s\t%20s\t%20s\t%20s\t%20s\n", "width", "processes", "real_time [s]", "user_time [s]", "sys_time [s]");
    printf("%20f\t%20d\t%20f\t%20f\t%20f\n\n", width, n, real_time, user_time, sys_time);

    char* filename = calloc(11, sizeof(char));
    sprintf(filename, "raport.txt");
    FILE* file = fopen(filename, "a");
    fprintf(file, "%20s\t%20s\t%20s\t%20s\t%20s\n", "width", "processes", "real_time [s]", "user_time [s]", "sys_time [s]");
    fprintf(file, "%20f\t%20d\t%20f\t%20f\t%20f\n\n", width, n, real_time, user_time, sys_time);
    fclose(file);
    free(filename);
#endif

    return 0;
}