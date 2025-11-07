#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>


#define N 5 

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N

int state[N];           
pthread_mutex_t mutex;  
pthread_cond_t self[N]; 

void test(int i) {
    if ((state[LEFT] != EATING) &&
        (state[i] == HUNGRY) &&
        (state[RIGHT] != EATING)) {
        state[i] = EATING;
        printf("Philosopher %d is EATING\n", i);

        pthread_cond_signal(&self[i]);
    }
}


void pickup(int i) {
    pthread_mutex_lock(&mutex);

    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);

    test(i);

    while (state[i] != EATING) {
        pthread_cond_wait(&self[i], &mutex);
    }

    pthread_mutex_unlock(&mutex);
}


void putdown(int i) {
    pthread_mutex_lock(&mutex);

    state[i] = THINKING;
    printf("Philosopher %d is THINKING\n", i);

    test(LEFT);
    test(RIGHT);

    pthread_mutex_unlock(&mutex);
}


void init_monitor() {
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        pthread_cond_init(&self[i], NULL);
    }
}

void *philosopher_life(void *num) {
    int i = *(int *)num;

    while (1) {
        sleep(rand() % 3 + 1);

        pickup(i);

        sleep(rand() % 3 + 1);

        putdown(i);
    }
}

int main() {
    pthread_t threads[N];
    int philosopher_ids[N];

    init_monitor();

    printf("Starting the Dining Philosophers simulation...\n");

    for (int i = 0; i < N; i++) {
        philosopher_ids[i] = i;
        if (pthread_create(&threads[i], NULL, philosopher_life, &philosopher_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;

}

