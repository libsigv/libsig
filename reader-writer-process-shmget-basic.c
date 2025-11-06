#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

typedef struct {
    sem_t wrt;
    pthread_mutex_t mutex;
    int data;
    int readcount;
} SharedData;

void writer(SharedData *shared, int id) {
    while (1) {
        sem_wait(&shared->wrt);
        
        shared->data = shared->data * 2;
        printf("Writer %d modified data to %d\n", id, shared->data);
        fflush(stdout);
        
        sem_post(&shared->wrt);
        sleep(2);
    }
}

void reader(SharedData *shared, int id) {
    while (1) {
        pthread_mutex_lock(&shared->mutex);
        shared->readcount++;
        if (shared->readcount == 1) {
            sem_wait(&shared->wrt);
        }
        pthread_mutex_unlock(&shared->mutex);
        
        printf("Reader %d read data as %d\n", id, shared->data);
        fflush(stdout);
        
        pthread_mutex_lock(&shared->mutex);
        shared->readcount--;
        if (shared->readcount == 0) {
            sem_post(&shared->wrt);
        }
        pthread_mutex_unlock(&shared->mutex);
        sleep(1);
    }
}

int main() {
    int num_readers = 5;
    int num_writers = 2;
    int shmid;
    key_t key = 1234;
    SharedData *shared;

    shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);

    shared = shmat(shmid, NULL, 0);

    shared->data = 1;
    shared->readcount = 0;

    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared->mutex, &mutex_attr);

    sem_init(&shared->wrt, 1, 1);

    pid_t pids[num_readers + num_writers];
    int child_count = 0;

    for (int i = 0; i < num_writers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            writer(shared, i + 1);
            shmdt(shared);
            exit(0);
        } else if (pid > 0) {
            pids[child_count++] = pid;
        }
    }

    for (int i = 0; i < num_readers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            reader(shared, i + 1);
            shmdt(shared);
            exit(0);
        } else if (pid > 0) {
            pids[child_count++] = pid;
        }
    }

    while(1) {
        sleep(10);
    }

    return 0;
}