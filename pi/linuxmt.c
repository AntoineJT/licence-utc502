#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// globals are bad, but this is a stupid sample program
// set once, never changed
int intervals;
double width;
// modified by threads intensively
double sum = 0;

// mutex + alive threads count
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int alive_threads = 0;

#define NUM_THREADS 2

void* pi_mt_ep(void* start_)
{
    int start = *(int*) start_;
    for (int i = start; i < intervals; i += NUM_THREADS)
    {
        double x = (i + 0.5) * width;
        double to_add = 4.0 / (1.0 + x * x);

        pthread_mutex_lock(&mutex);
        sum += to_add;
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    --alive_threads;
    pthread_mutex_unlock(&mutex);

    free(start_);
}

double pi_mt_main(void)
{
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        int* start = malloc(sizeof(int));
        *start = i;
        pthread_create(&threads[i], NULL, pi_mt_ep, (void*) start);
        
        pthread_mutex_lock(&mutex);
        ++alive_threads;
        pthread_mutex_unlock(&mutex);
    }

    while(alive_threads > 0) sleep(1);
    
    return sum * width;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("Usage: %s [intervals]\n"
               "Computes a pi estimation\n", argv[0]);
        return EXIT_FAILURE;
    }

    // atoi -> interprets a str to int
    intervals = atoi(argv[1]);
    width = 1.0 / intervals;
    
    double pi = pi_mt_main();
    printf("Pi estimation - %10.9f\n", pi);

    return EXIT_SUCCESS;
}
