//
// Created by bchwast on 12/05/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

int thread_count;

typedef struct image {
    int height;
    int width;
    int** pixels;
} image;


typedef struct thread {
    image* img;
    image* negative;
    int index;
} thread;


int** alloc_matrix(int height, int width) {
    int** matrix = (int **) calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++) {
        matrix[i] = (int *) calloc(width, sizeof(int));
    }

    return matrix;
}


void free_matrix(int** matrix, int height) {
    for (int i = 0; i <= height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


image* load_image(char* input_name) {
    FILE* input = fopen(input_name, "r");
    if (input == NULL) {
        perror("Error: could not open input file\n");
        exit(EXIT_FAILURE);
    }

    int m;
    image* img = (image *) calloc(1, sizeof(image));
    fscanf(input, "P2\n%d %d\n%d\n", &img->width, &img->height, &m);

    if (m != 255) {
        perror("Error: wrong color depth\n");
        exit(EXIT_FAILURE);
    }

    img->pixels = alloc_matrix(img->height, img->width);
    int buff;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            fscanf(input, "%d", &buff);
            img->pixels[y][x] = buff;
        }
    }

    fclose(input);
    return img;
}


image* copy_img(image* src) {
    image* dest = (image *) calloc(1, sizeof(image *));
    dest->width = src->width;
    dest->height = src->height;
    dest->pixels = alloc_matrix(dest->height, dest->width);

    for (int y = 0; y < dest->height; y++) {
        for (int x = 0; x < dest->width; x++) {
            dest->pixels[y][x] = src->pixels[y][x];
        }
    }

    return dest;
}


void save_image(image* img, char* output_name) {
    FILE* output = fopen(output_name, "w");
    if (output == NULL) {
        perror("Error: could not create output file\n");
        exit(EXIT_FAILURE);
    }

    fprintf(output, "P2\n");
    fprintf(output, "%d %d\n", img->width, img->height);
    fprintf(output, "255\n");

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width - 1; x++) {
            fprintf(output, "%d ", img->pixels[y][x]);
        }
        fprintf(output, "%d\n", img->pixels[y][img->width - 1]);
    }

    fclose(output);
}


long int* numbers(thread* thread_args) {
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    image* img = thread_args->img;
    image* negative = thread_args->negative;
    int index = thread_args->index;

    int min_val = index * ceil(256.0 / (double) thread_count);
    int max_val = (index != thread_count - 1) ? (index + 1) * ceil(256.0 / (double) thread_count) : 256;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            if (img->pixels[y][x] >= min_val && img->pixels[y][x] < max_val) {
                negative->pixels[y][x] = 255 - img->pixels[y][x];
            }
        }
    }

    gettimeofday(&stop, NULL);
    long int* time = (long int *) calloc(1, sizeof(long int));
    *time = 1000000*(stop.tv_sec-start.tv_sec) + (stop.tv_usec - start.tv_usec);
    pthread_exit(time);
}


long int* block(thread* thread_args) {
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    image* img = thread_args->img;
    image* negative = thread_args->negative;
    int index = thread_args->index;

    int min_val = index * ceil(img->width / (double) thread_count);
    int max_val = (index != thread_count - 1) ? (index + 1) * ceil(img->width / (double) thread_count) : img->width;

    for (int y = 0; y < img->height; y++) {
        for (int x = min_val; x < max_val; x++) {
            negative->pixels[y][x] = 255 - img->pixels[y][x];
        }
    }

    gettimeofday(&stop, NULL);
    long int* time = (long int *) calloc(1, sizeof(long int));
    *time = 1000000*(stop.tv_sec-start.tv_sec) + (stop.tv_usec - start.tv_usec);
    pthread_exit(time);
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        perror("Error: expected 4 arguments [thread no] [numbers/block] [input] [output]\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[2], "numbers") != 0 && strcmp(argv[2], "block") != 0) {
        perror("Error: wrong mode, expected [numbers/block]\n");
        exit(EXIT_FAILURE);
    }

    thread_count = atoi(argv[1]);
    char* mode = strdup(argv[2]);
    char* input_name = strdup(argv[3]);
    char* output_name = strdup(argv[4]);

    image* img = load_image(input_name);
    image* negative = copy_img(img);

    pthread_t* threads = (pthread_t *) calloc(thread_count, sizeof(pthread_t));
    thread* thread_args = (thread *) calloc(thread_count, sizeof(thread));
    for (int i = 0; i < thread_count; i++) {
        thread_args[i].img = img;
        thread_args[i].negative = negative;
        thread_args[i].index = i;
    }
    long int** exec_times = (long int **) calloc(thread_count, sizeof(long int *));
    for (int i = 0; i < thread_count; i++) {
        exec_times[i] = (long int *) calloc(1, sizeof(long int));
    }

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    if (strcmp(mode, "numbers") == 0) {
        for (int i = 0; i < thread_count; i++) {
            pthread_create(&threads[i], NULL, (void * (*)(void *)) numbers, &thread_args[i]);
        }
    }
    else if (strcmp(mode, "block") == 0) {
        for (int i = 0; i < thread_count; i++) {
            pthread_create(&threads[i], NULL, (void * (*)(void *)) block, &thread_args[i]);
        }
    }
    else {
        perror("Error: unknown traverse mode\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], (void *) &exec_times[i]);
    }

    gettimeofday(&stop, NULL);
    long int total_time = 1000000*(stop.tv_sec - start.tv_sec) + (stop.tv_usec - start.tv_usec);
    save_image(negative, output_name);

    printf("Thread count: %3d, traverse mode: %7s, file: %s\n", thread_count, mode, input_name);
    for (int i = 0; i < thread_count; i++) {
        printf("thread: %3d\ttime: %7lu [µs]\n", i, *(exec_times[i]));
    }
    printf("Total time: %5lu [µs]\n", total_time);

    free(threads);
    for (int i = 0; i < thread_count; i++) {
        free(exec_times[i]);
    }
    free(exec_times);

    free(thread_args);
    free_matrix(negative->pixels, negative->height);
    free(negative);
    free_matrix(img->pixels, img->height);
    free(img);

    return 0;
}