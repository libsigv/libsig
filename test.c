#include <semaphore.h>
#include <stdio.h>

sem_t sema;

int main() {
    sem_init(&sema, 0, 1);
    sem_wait(&sema);

    printf("Hello");

    sem_post(&sema);
}