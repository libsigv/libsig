#include <stdio.h>
#include <pthread.h>

int flag[2] = {0, 0};
int turn;
int shared_resource = 0;

void* process(void* arg) {
    long id = (long)arg;
    int other = 1 - (int)id;

    flag[id] = 1;
    turn = other;
    while (flag[other] == 1 && turn == other) {
    }

    printf("Process %ld entering critical section.\n", id);
    shared_resource++;
    printf("Process %ld exiting critical section. Shared resource: %d\n", id, shared_resource);

    flag[id] = 0;

    return NULL;
}

int main() {
    pthread_t tid[2];

    pthread_create(&tid[0], NULL, process, (void*)0);
    pthread_create(&tid[1], NULL, process, (void*)1);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    printf("Final shared resource value: %d\n", shared_resource);

    return 0;
}
