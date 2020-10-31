#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 80

// globals
char buffer[BUFFER_SIZE] = {0}; // threads' shared buffer 

// mutex + alive threads count
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int alive_threads = 0;

int in_buffer = 0;

void atomic_inc(int* ptr)
{
    pthread_mutex_lock(&mutex);
    ++*ptr;
    pthread_mutex_unlock(&mutex);
}

void atomic_dec(int* ptr)
{
    pthread_mutex_lock(&mutex);
    --*ptr;
    pthread_mutex_unlock(&mutex);
}

// reader thread
void* r_thread(void* unused)
{
    atomic_inc(&alive_threads);
    
    while(in_buffer < BUFFER_SIZE) 
    {
        switch(read(STDIN_FILENO, buffer, 1))
        {
            case 1: atomic_inc(&in_buffer); break;
            case 0: break;
            case -1: goto r_exit; break;
        }
    }

r_exit: 
    atomic_dec(&alive_threads);
}

// writer thread
void* w_thread(void* unused)
{
    atomic_inc(&alive_threads);
    
    while(in_buffer > 0)
    {
        switch(write(STDOUT_FILENO, buffer, 1))
        {
            case 1: atomic_dec(&in_buffer); break;
            case 0: break;
            case -1: goto w_exit; break;
        }
    }

w_exit:
    atomic_dec(&alive_threads);
}

int main(void)
{
    pthread_t threads[2];

    pthread_create(&threads[0], NULL, r_thread, NULL);
    pthread_create(&threads[1], NULL, w_thread, NULL);

    while(alive_threads > 0) sleep(1);

    return EXIT_SUCCESS;
}
