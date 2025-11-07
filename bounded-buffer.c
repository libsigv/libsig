#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define ITEMS_TO_PRODUCE 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

int buffer[BUFFER_SIZE]; 
int in = 0;              
int out = 0;            

sem_t empty;             
sem_t full;             
pthread_mutex_t mutex;  

typedef struct {
    int id;
} ThreadArgs;


void* producer(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int producer_id = args->id;
    int item;

    for (int i = 0; i < ITEMS_TO_PRODUCE; i++) {
        item = producer_id * 100 + i;

        sem_wait(&empty);
        
        pthread_mutex_lock(&mutex);

        buffer[in] = item;
        printf("Producer %d: Produced item %d at index %d\n", producer_id, item, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        
        sem_post(&full);
        sleep(rand() % 2);
    }
    
    printf("Producer %d: Finished producing.\n", producer_id);
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int consumer_id = args->id;
    int item;

    for (int i = 0; i < (NUM_PRODUCERS * ITEMS_TO_PRODUCE) / NUM_CONSUMERS; i++) {
        
        sem_wait(&full);
        
        pthread_mutex_lock(&mutex);

        item = buffer[out];
        printf("Consumer %d: Consumed item %d from index %d\n", consumer_id, item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        
        sem_post(&empty);
        sleep(rand() % 3);
    }

    printf("Consumer %d: Finished consuming.\n", consumer_id);
    pthread_exit(NULL);
}

int main() {
    pthread_t prod_threads[NUM_PRODUCERS];
    pthread_t cons_threads[NUM_CONSUMERS];
    ThreadArgs prod_args[NUM_PRODUCERS];
    ThreadArgs cons_args[NUM_CONSUMERS];

    srand(time(NULL));

    
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        prod_args[i].id = i;
        if (pthread_create(&prod_threads[i], NULL, producer, &prod_args[i]) != 0) {
            perror("Failed to create producer thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cons_args[i].id = i;
        if (pthread_create(&cons_threads[i], NULL, consumer, &cons_args[i]) != 0) {
            perror("Failed to create consumer thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("All producers and consumers have finished.\n");

    return 0;
}
