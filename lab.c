
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define SUCCESS 0
#define max_order_of_threads 5
#define max_order_of_iterations 8
#define num_of_args 3
#define min_count_of_thread 1
#define max_count_of_iterations 1000000000
#define basis 10

#define ERR_OF_NUM_OF_ARGS 1
#define ERR_OF_NUM_OF_THREADS 2
#define ERR_OF_FORM_OF_ARGS 3
#define ERR_OF_NUM_OF_ITERATIONS 4
#define ERR_OF_JOINING_OF_THREAD 5

typedef struct param_of_thread {
    int count_of_threads;
    int number_of_thread;
    int count_of_iterations;
    double part_of_pi;
} param_of_thread;

int isCorrect(int a, char* b) {
    char buf[1024];
    sprintf(buf, "%d", a);
    return !strcmp(buf, b);
}

void* startWork(void* param) {
    if (param == NULL) {
        fprintf(stderr, "Wrong parameters of thread\n");
        return NULL;
    }

    param_of_thread* current_param = (param_of_thread*)param;
    double part_of_pi = 0;

    for(int i = current_param->number_of_thread; i < current_param->count_of_iterations; i += current_param->count_of_threads) {
        part_of_pi += 1.0 / (i * 4.0 + 1.0);
        part_of_pi -= 1.0 / (i * 4.0 + 3.0);
    }

    current_param->part_of_pi = part_of_pi;
    return param;
}

int checkOfErrors(int argc, char** argv, int* count_of_threads, int* count_of_iterations) {
    if(argc != num_of_args) {
        fprintf(stderr, "Wrong count of args\n");
        return ERR_OF_NUM_OF_ARGS;
    }

    if(strlen(argv[1]) > max_order_of_threads) {
        fprintf(stderr, "Too many threads\n");
        return ERR_OF_NUM_OF_THREADS;
    }

    *count_of_threads = strtol(argv[1], (char**)NULL, basis);

    if(!isCorrect(*count_of_threads, argv[1])) {
        fprintf(stderr, "Use digits only\n");
        return ERR_OF_FORM_OF_ARGS;
    }

    if(*count_of_threads < min_count_of_thread) {
        fprintf(stderr, "Too few threads\n");
        return ERR_OF_NUM_OF_THREADS;
    }

    if(strlen(argv[2]) > max_order_of_iterations) {
        fprintf(stderr, "Too many iterations\n");
        return ERR_OF_NUM_OF_ITERATIONS;
    }

    *count_of_iterations = strtol(argv[2], (char**)NULL, basis);

    if(*count_of_iterations < *count_of_threads) {
        fprintf(stderr, "Too few iterations\n");
        return ERR_OF_NUM_OF_ITERATIONS;
    }

    if(!isCorrect(*count_of_iterations, argv[2])) {
        fprintf(stderr, "Use digits only\n");
        return ERR_OF_FORM_OF_ARGS;
    }

    return SUCCESS;
}

int calculateOfPi(int* count_of_threads, int* count_of_iterations, double* pi) {
    pthread_t threads[*count_of_threads];
    param_of_thread param[*count_of_threads];
    int count_of_created_threads = *count_of_threads;

    for (int i = 0; i < *count_of_threads; i++) {
        param[i].count_of_threads = *count_of_threads;
        param[i].number_of_thread = i;
        param[i].count_of_iterations = *count_of_iterations;
        errno = pthread_create(&threads[i], NULL, startWork, (void*)(&param[i]));
        if (errno != SUCCESS) {
            count_of_created_threads = i;
            perror("Error of creating of thread");
            break;
        }
    }
    for (int i = 0; i < count_of_created_threads; i++) {
        errno = pthread_join(threads[i], NULL);
        if (errno != SUCCESS) {
            perror("Error of joining of thread");
            return ERR_OF_JOINING_OF_THREAD;
        }
        *pi += param[i].part_of_pi;
    }

    *pi *= 4.0;
    return SUCCESS;
}

int main(int argc, char** argv) {
    int number_of_error,
            count_of_threads,
            count_of_iterations;

    number_of_error = checkOfErrors(argc, argv, &count_of_threads, &count_of_iterations);
    if (number_of_error != SUCCESS) {
        return number_of_error;
    }

    double pi = 0;

    number_of_error = calculateOfPi(&count_of_threads, &count_of_iterations, &pi);
    if (number_of_error != SUCCESS) {
        return number_of_error;
    }

    printf("Calculate of pi done, result = %.15g \n", pi);

    return EXIT_SUCCESS;
}
