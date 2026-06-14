#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_READERS 3
#define NUM_WRITERS 2
#define ITERATIONS 10

int data = 0;

int readcount = 0;
int writecount = 0;

sem_t x;
sem_t y;
sem_t z;
sem_t rsem;
sem_t wsem;

void* reader(void* arg)
{
    int id = *(int*)arg;

    for(int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&z);

        sem_wait(&rsem);

        sem_wait(&x);

        readcount++;

        if(readcount == 1)
        {
            sem_wait(&wsem);
        }

        sem_post(&x);

        sem_post(&rsem);

        sem_post(&z);

        printf("[Reader %d] Reading Data = %d (Readers = %d)\n",
               id, data, readcount);

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
    int id = *(int*)arg;

    for(int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&y);

        writecount++;

        if(writecount == 1)
        {
            sem_wait(&rsem);
        }

        sem_post(&y);

        sem_wait(&wsem);

        data++;

        printf("[Writer %d] Writing Data = %d\n",
               id, data);

        sem_post(&wsem);

        sem_wait(&y);

        writecount--;

        if(writecount == 0)
        {
            sem_post(&rsem);
        }

        sem_post(&y);
    }

    return NULL;
}

int main()
{
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    int reader_ids[NUM_READERS];
    int writer_ids[NUM_WRITERS];

    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);
    sem_init(&z, 0, 1);
    sem_init(&rsem, 0, 1);
    sem_init(&wsem, 0, 1);

    for(int i = 0; i < NUM_READERS; i++)
    {
        reader_ids[i] = i;

        pthread_create(&readers[i],
                       NULL,
                       reader,
                       &reader_ids[i]);
    }

    for(int i = 0; i < NUM_WRITERS; i++)
    {
        writer_ids[i] = i;

        pthread_create(&writers[i],
                       NULL,
                       writer,
                       &writer_ids[i]);
    }

    for(int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    for(int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    printf("\n===== FINAL RESULT =====\n");
    printf("Final Data = %d\n", data);

    sem_destroy(&x);
    sem_destroy(&y);
    sem_destroy(&z);
    sem_destroy(&rsem);
    sem_destroy(&wsem);

    return 0;
}
