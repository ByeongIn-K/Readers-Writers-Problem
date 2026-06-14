#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_READERS 3
#define NUM_WRITERS 2
#define ITERATIONS 10

int data = 0;
int readcount = 0;

sem_t x;
sem_t wsem;

void* reader(void* arg)
{
    int id = *(int*)arg;

    for(int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&x);

        readcount++;

        if(readcount == 1)
        {
            sem_wait(&wsem);
        }

        sem_post(&x);

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
        sem_wait(&wsem);

        data++;

        printf("[Writer %d] Writing Data = %d\n",
               id, data);

        sem_post(&wsem);
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
    sem_destroy(&wsem);

    return 0;
}
