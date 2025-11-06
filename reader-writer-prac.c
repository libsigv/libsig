#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t sema;
sem_t lock;
int readers;
void *reader(void *i) {
    int ind = (int)i;

    sem_wait(&lock);
    readers++;
    if (readers == 1) {
        sem_wait(&sema);
    }
    sem_post(&lock);

    // reads

    sem_wait(&lock);
    readers--;
    if (readers == 0) {
        sem_post(&sema);
    }
    sem_post(&lock);
}

void *writer(void *i) {
    int ind = (int)i;

    sem_wait(&sema);

    // write

    sem_post(&sema);
}

int main() {
    sem_init(&sema, 0, 1);
    sem_init(&lock, 0, 1);

    pthread_t read[5];
    pthread_t write[5];

    for (int i = 0; i < 5; i++) {
        pthread_create(&read[i], NULL, reader, (void *)i);
    }

    for (int i = 0; i < 5; i++) {
        pthread_create(&write[i], NULL, writer, (void *)i);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }
}
