//
// Created by bchwast on 11/03/2022.
//

#include "library.h"
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef DLL
    #include <dlfcn.h>
#endif


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


int correctOperation(char* operation){
    return strcmp(operation, "create_array") *
           strcmp(operation, "wc_files") *
           strcmp(operation, "remove_block") *
           strcmp(operation, "print_array") *
           strcmp(operation, "start_measurement") *
           strcmp(operation, "end_measurement") *
           strcmp(operation, "print_header") *
           strcmp(operation, "remove_array") *
           strcmp(operation, "help");
}

void help() {
    printf("Available commands:\n");
    printf("create_array [size of array]\n");
    printf("wc_files [file names separated with space inside \"\"]\n");
    printf("remove_array [index]\n");
    printf("print_array\n");
    printf("remove_array\n");
    printf("start_measurement [commands] end_measurement [message]\n");
    printf("print_header\n");
    printf("help\n");
}

int main(int argc, char* argv[]) {
    #ifdef DLL
        void* handle = dlopen("./liblibrary.so", RTLD_LAZY);
        if (handle == NULL) {
            printf("Handle to shared library has not beem found\n");
            exit(0);
        }

        void (*create_array)(unsigned int) = dlsym(handle, "create_array");
        int (*wc_files)(char*) = dlsym(handle, "wc_files");
        void (*remove_block)(int) = dlsym(handle, "remove_block");
        void (*print_array)(void) = dlsym(handle, "print_array");
        void (*remove_array)(void) = dlsym(handle, "remove_array");
    #endif


    for (int i = 1; i < argc; i++) {

        if (correctOperation(argv[i]) != 0) {
            printf("Unknown command\n");
            exit(0);
        }

        if (strcmp(argv[i], "create_array") == 0) {
            int size_ = atoi(argv[++i]);
            create_array(size_);
        }

        else if (strcmp(argv[i], "wc_files") == 0) {
            wc_files(argv[++i]);
        }

        else if (strcmp(argv[i], "remove_block") == 0) {
            remove_block(atoi(argv[++i]));
        }

        else if (strcmp(argv[i], "print_array") == 0) {
            print_array();
        }

        else if (strcmp(argv[i], "start_measurement") == 0){
            start_measurement();
        }

        else if (strcmp(argv[i], "end_measurement") == 0){
            end_and_print_measurement(argv[++i]);
        }

        else if (strcmp(argv[i], "print_header") == 0){
            print_header();
        }

        else if (strcmp(argv[i], "remove_array") == 0) {
            remove_array();
        }

        else if (strcmp(argv[i], "help") == 0) {
            help();
        }
    }

    #ifdef DLL
        dlclose(handle);
    #endif

}