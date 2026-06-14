#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_READERS 8
#define NUM_WRITERS 2
#define ITERATIONS 1000000

int data = 0;
int readcount = 0;

long long total_reads = 0;
long long total_writes = 0;

sem_t x;
sem_t wsem;

void* reader(void* arg)
{
    for(int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&x);

        readcount++;

        if(readcount == 1)
        {
            sem_wait(&wsem);
        }

        sem_post(&x);

        int value = data;
        (void)value;

        __sync_fetch_and_add(&total_reads, 1);

        sem_wait(&x);

        readcount--;

        if(readcount == 0)
        {
            sem_post(&wsem);
        }

        sem_post(&x);
    }

    return NULL;
}

void* writer(void* arg)
{
    for(int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&wsem);

        data++;

        __sync_fetch_and_add(&total_writes, 1);

        sem_post(&wsem);
    }

    return NULL;
}

int main()
{
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    struct timespec start, end;

    sem_init(&x, 0, 1);
    sem_init(&wsem, 0, 1);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for(int i = 0; i < NUM_READERS; i++)
    {
        pthread_create(&readers[i], NULL, reader, NULL);
    }

    for(int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_create(&writers[i], NULL, writer, NULL);
    }

    for(int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    for(int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("===== Reader Preference Result =====\n");
    printf("Total Reads  : %lld\n", total_reads);
    printf("Total Writes : %lld\n", total_writes);
    printf("Final Data   : %d\n", data);
    printf("Execution Time : %.6f sec\n", elapsed);

    sem_destroy(&x);
    sem_destroy(&wsem);

    return 0;
}
