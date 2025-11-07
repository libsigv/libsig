#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

sem_t rc, wrt, mutex;
int data = 0, read_cunt = 0, wc = 0;

void *reader(void *arg) {
    int id = (int)arg;

    while (1) {
        sem_wait(&mutex);
        while (wc > 0) {
            sem_post(&mutex);
            sleep(1);
            sem_wait(&mutex);
        }
        sem_post(&mutex);

        sem_wait(&rc);
        read_cunt++;

        if (read_cunt == 1) {
            sem_wait(&wrt);
        }
        sem_post(&rc);

        printf("Reader %d is reading %d !!\n", id, data);
        sleep(1);

        sem_wait(&rc);
        read_cunt--;
        if (read_cunt == 0) {
            sem_post(&wrt);
        }
        sem_post(&rc);

        printf("Reading finished!!\n");
        sleep(1);
    }
}

void *writer(void *arg) {
    int id = (int)arg;

    while (1) {
        sem_wait(&mutex);
        wc++;
        sem_post(&mutex);

        sem_wait(&wrt);
        data++;
        printf("Writer %d is writing data to %d !!\n", id, data);
        sleep(1);
        sem_post(&wrt);

        sem_wait(&mutex);
        wc--;
        sem_post(&mutex);
        printf("Writing finished !!\n");
        sleep(1);
    }
}

int main() {

    pthread_t r[5], w[3];
    int rid[5], wid[3];

    sem_init(&rc, 0, 1);
    sem_init(&wrt, 0, 1);
    sem_init(&mutex, 0, 1);

    for (int i = 0; i < 5; i++) {
        rid[i] = i + 1;
        pthread_create(&r[i], NULL, reader, &rid[i]);
    }

    for (int i = 0; i < 3; i++) {
        wid[i] = i + 1;
        pthread_create(&w[i], NULL, writer, &wid[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(r[i], NULL);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(w[i], NULL);
    }

    sem_destroy(&rc);
    sem_destroy(&wrt);
    sem_destroy(&mutex);
    return 0;
}