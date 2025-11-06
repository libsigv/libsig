#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
int phil_ids[N];
pthread_mutex_t mutex;
pthread_cond_t cond[N];

void test(int i) {
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        printf("Philosopher %d is Eating.\n", i + 1);
        pthread_cond_signal(&cond[i]);
    }
}

void pickup_forks(int i) {
    pthread_mutex_lock(&mutex);

    state[i] = HUNGRY;
    printf("Philosopher %d is Hungry.\n", i + 1);
    test(i);

    while (state[i] == HUNGRY) {
        pthread_cond_wait(&cond[i], &mutex);
    }

    pthread_mutex_unlock(&mutex);
}

void putdown_forks(int i) {
    pthread_mutex_lock(&mutex);

    state[i] = THINKING;
    printf("Philosopher %d is Thinking.\n", i + 1);

    test(LEFT);
    test(RIGHT);

    pthread_mutex_unlock(&mutex);
}

void *philosopher(void *num) {
    int i = *(int *)num;
    while (1) {
        sleep(rand() % 3 + 1);
        pickup_forks(i);
        sleep(rand() % 3 + 1);
        putdown_forks(i);
    }
}

int main() {
    pthread_t thread_id[N];

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < N; i++) {
        phil_ids[i] = i;
        pthread_cond_init(&cond[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&thread_id[i], NULL, philosopher, &phil_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(thread_id[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < N; i++) {
        pthread_cond_destroy(&cond[i]);
    }

    return 0;
}