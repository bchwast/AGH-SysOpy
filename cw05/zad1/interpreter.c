//
// Created by bchwast on 07/04/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <ctype.h>

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
    char** names;
    size_t components_n;
} execution;


char *trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}


int get_num_of_args(char* line) {
    int i = 0;
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part = strtok(line_cpy, " ");

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


program get_program(char* line_) {
    char* name = (char *) calloc(LINE_LEN, sizeof(char));
    size_t arguments_n = get_num_of_args(line_) + 1;
    char** arguments = (char **) calloc(arguments_n, sizeof(char *));

    int i = 0;
    char* line_cpy_ = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy_, line_);
    char* _part = strtok(line_cpy_, " ");
    strcpy(name, trimwhitespace(_part));

    do {
        arguments[i] = trimwhitespace(_part);
        i++;
    }
    while((_part = strtok(NULL, " ")) != NULL);
    free(_part);

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
    strcpy(name, trimwhitespace(part));
    part = strtok(NULL, "=");
    char* part__ = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(part__, part);
    char* part_ = strtok(part__, "|");
    char* part_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    do {
        strcpy(part_cpy, part_);
        programs[i] = get_program(part_cpy);
        i++;
        strcpy(part__ ,part);
        part_ = strtok(part__, "|");
        for (int j = 0; j < i; j++) {
            part_ = strtok(NULL, "|");;
        }
    } while(part_ != NULL);
    free(part_);
    free(part__);

    component component_ = { .programs = programs, .programs_n = programs_n, .name = name };

    return component_;
}


int get_num_of_components_to_execute(char* line) {
    int num = 0;
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part;
    if ((part = strtok(line_cpy, "|")) != NULL) {
        num++;
    }
    while ((part = strtok(NULL, "|")) != NULL) {
        num++;
    }
    free(part);
    return num;
}


execution get_execution(char* line) {
    size_t components_n = get_num_of_components_to_execute(line);
    char** names = (char **) calloc(components_n, sizeof(char *));
    char* line_cpy = (char *) calloc(LINE_LEN, sizeof(char));
    strcpy(line_cpy, line);
    char* part = strtok(line_cpy, "|");
    int i = 0;
    do {
        names[i] = trimwhitespace(part);
        i++;
    }
    while ((part = strtok(NULL, "|")) != NULL);
    free(part);

    execution execution_ = { .names = names, .components_n = components_n};
    return execution_;
}


int get_component_index(component* components, int num_of_components, char* name) {
    int i = 0;
    while (i < num_of_components) {
        if (strcmp(name, components[i].name) == 0) {
            return i;
        }
        i++;
    }
    return -1;
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

    while (fgets(line, LINE_LEN * sizeof(char), source)) {
        execution execution_ = get_execution(line);
        free(line);
        line = (char *) calloc(LINE_LEN, sizeof(char));

        int pipe_in[2];
        int pipe_out[2];

        if (pipe(pipe_out) != 0) {
            printf("Error while creating a pipe\n");
            exit(1);
        }


        for (int i = 0; i < execution_.components_n; i++) {
            int m = get_component_index(components, num_of_components, execution_.names[i]);
            for (int j = 0; j < components[m].programs_n; j++) {
                pid_t pid = fork();
                if (pid < 0) {
                    printf("Could not fork\n");
                    exit(1);
                }

                if (pid == 0) {
                    if (j == 0 && i == 0) {
                        close(pipe_out[READ]);
                        dup2(pipe_out[WRITE], STDOUT_FILENO);
                    } else if (j == components[m].programs_n - 1 && i == execution_.components_n - 1) {
                        close(pipe_out[READ]);
                        close(pipe_out[WRITE]);
                        close(pipe_in[WRITE]);
                        dup2(pipe_in[READ], STDIN_FILENO);
                    } else {
                        close(pipe_in[WRITE]);
                        close(pipe_out[READ]);
                        dup2(pipe_in[READ], STDIN_FILENO);
                        dup2(pipe_out[WRITE], STDOUT_FILENO);
                    }

                    if (execvp(components[m].programs[j].name, components[m].programs[j].arguments) == -1) {
                        printf("Error in exec\n");
                        exit(1);
                    }
                } else {
                    close(pipe_in[WRITE]);
                    pipe_in[READ] = pipe_out[READ];
                    pipe_in[WRITE] = pipe_out[WRITE];

                    if (pipe(pipe_out) != 0) {
                        printf("Error while moving pipe\n");
                        exit(1);
                    }
                }
            }
        }

        int wstatus;
        while (wait(&wstatus) > 0) {
            int exit_status;
            if (WIFEXITED(wstatus) && (exit_status = WEXITSTATUS(wstatus)) != 0) {
                fprintf(stderr, "MISSION ABORT, SOME PROCESS RETURNED NONZERO CODE: %d\n", exit_status);
                raise(SIGTERM);
            }
        }
    }
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
