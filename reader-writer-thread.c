#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t wrt;
pthread_mutex_t mutex;
int data = 1;
int readcount = 0;

void *writer(void *arg) {
    int id = *((int *)arg);
    while (1) {
        sem_wait(&wrt);

        data = data * 2;
        printf("Writer %d modified data to %d\n", id, data);

        sem_post(&wrt);
        sleep(2);
    }
    return NULL;
}

void *reader(void *arg) {
    int id = *((int *)arg);
    while (1) {
        pthread_mutex_lock(&mutex);
        readcount++;
        if (readcount == 1) {
            sem_wait(&wrt);
        }
        pthread_mutex_unlock(&mutex);

        printf("Reader %d read data as %d\n", id, data);

        pthread_mutex_lock(&mutex);
        readcount--;
        if (readcount == 0) {
            sem_post(&wrt);
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    int num_readers = 5;
    int num_writers = 2;

    pthread_t readers[num_readers];
    pthread_t writers[num_writers];

    int reader_ids[num_readers];
    int writer_ids[num_writers];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&wrt, 0, 1);

    for (int i = 0; i < num_writers; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &writer_ids[i]);
    }

    for (int i = 0; i < num_readers; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    for (int i = 0; i < num_writers; i++) {
        pthread_join(writers[i], NULL);
    }

    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}