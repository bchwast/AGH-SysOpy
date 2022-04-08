//
// Created by bchwast on 07/04/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define MAX_LINES 10
#define MAX_ARGS 20
#define LINE_LEN 2000
#define READ 0
#define WRITE 1


typedef struct {
    char* name;
    char** arguments;
    size_t arguments_n;
} program;

typedef struct {
    program* programs;
    size_t programs_n;
    char* name;
} component;

typedef struct {
    component* components;
    size_t components_n;
} execution;


//char** parse_source(char* line) {
//    char** source = (char **) calloc(MAX_ARGS, sizeof(char *));
//
//    int i = 0;
//    char* part = strtok(line, "=");
//
//    while ((part = strtok(NULL, "|")) != NULL) {
//        source[i] = part;
//        i++;
//    }
//
//    free(part);
//
//    return source;
//}
//
//
//char** get_definitions(FILE* source) {
//    char** definitions = (char **) calloc(MAX_LINES, sizeof(char *));
//    char* line = (char *) calloc(LINE_LEN, sizeof(char));
//
//    int line_cnt = 0;
//    while (fgets(line, LINE_LEN * sizeof(char), source) && strstr(line, "=")) {
//        definitions[line_cnt] = (char *) calloc(LINE_LEN, sizeof(char));
//        strcpy(definitions[line_cnt], line);
//        line_cnt++;
//        free(line);
//        line = (char *) calloc(LINE_LEN, sizeof(char));
//    }
//    free(line);
//
//    return definitions;
//}
//
//
//int* get_components_num(char* components_list) {
//
//}


int get_num_of_args(char* line) {
    int i = 0;
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part = strtok(line_cpy, " ");
//    part = strtok(NULL, " ");

    while ((part = strtok(NULL, " ")) != NULL) {
        i++;
    }

    free(part);
    return i;
}


int get_num_of_programs(char* line) {
    int i = 0;
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part = strtok(line_cpy, "=");

    while ((part = strtok(NULL, "|")) != NULL) {
        i++;
    }

    free(part);
    return i;
}


program get_program(char* line) {
    char* name = (char *) calloc(LINE_LEN, sizeof(char));
    size_t arguments_n = get_num_of_args(line);
    char** arguments = (char **) calloc(arguments_n, sizeof(char *));

    int i = 0;
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part = strtok(line_cpy, " ");
    strcpy(name, part);

    while((part = strtok(NULL, " ")) != NULL) {
        arguments[i] = part;
        i++;
    }
    free(part);

    program program_ = { .arguments_n = arguments_n, .name = name, .arguments = arguments };
    return program_;
}


int get_num_of_components(FILE* source) {
    char* line = calloc(2000, sizeof(char));
    int num = 0;

    while (fgets(line, LINE_LEN * sizeof(char), source) && strstr(line, "=")) {
        num++;
        free(line);
        line = calloc(2000, sizeof(char));
    }
    free(line);
    fseek(source, 0, SEEK_SET);

    return num;
}


component get_component(char* line) {
    char* name = (char *) calloc(LINE_LEN, sizeof(char));
    size_t programs_n = get_num_of_programs(line);
    program* programs = calloc(programs_n, sizeof(program));

    int i = 0;
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part = strtok(line_cpy, "=");
    strcpy(name, part);

    while((part = strtok(NULL, "|")) != NULL) {
//        printf("%s %s\n", line, part);
        programs[i] = get_program(part);
        i++;
    }
    free(part);

    component component_ = { .programs = programs, .programs_n = programs_n, .name = name };

    return component_;
}


void interpret(FILE* source) {
    int num_of_components = get_num_of_components(source);
    component* components = calloc(num_of_components, sizeof(component));

    char* line = (char *) calloc(LINE_LEN, sizeof(char));
    for (int i = 0; i < num_of_components; i++) {
        fgets(line, LINE_LEN * sizeof(char), source);
        components[i] = get_component(line);
        free(line);
        line = (char *) calloc(LINE_LEN, sizeof(char));
    }

    
    
//    char** definitions = get_definitions(source);
//    int* components_numbers;
//
//    char* line = (char *) calloc(LINE_LEN, sizeof(char));
//
//    int line_cnt = 0
//    while (fgets(line, LINE_LEN * sizeof(char), source)) {
//        components_num = get_components_num
//    }


}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("You need to specify the source file\n");
        exit(1);
    }

    char* file_name = argv[1];
    FILE* source = fopen(file_name, "r");
    if (source == NULL) {
        printf("Could not open the source file\n");
        exit(1);
    }

    interpret(source);

    fclose(source);
}
