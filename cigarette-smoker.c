#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>     
#include <time.h>       

sem_t agent_sem;

sem_t smoker_tobacco_sem;
sem_t smoker_paper_sem;
sem_t smoker_matches_sem;

const char* items[] = {"TOBACCO", "PAPER", "MATCHES"};

void* agent_thread(void* arg) {
    while (1) {
        sem_wait(&agent_sem);

        int item1 = rand() % 3;
        int item2;
        do {
            item2 = rand() % 3;
        } while (item1 == item2);


        if (item1 != 0 && item2 != 0) {
            printf("AGENT: Places %s and %s on the table.\n", items[1], items[2]);
            sem_post(&smoker_tobacco_sem); 
        }
        else if (item1 != 1 && item2 != 1) {
            printf("AGENT: Places %s and %s on the table.\n", items[0], items[2]);
            sem_post(&smoker_paper_sem); 
        } 
        else {
            printf("AGENT: Places %s and %s on the table.\n", items[0], items[1]);
            sem_post(&smoker_matches_sem); 
        }
    }
    return NULL;
}

void* smoker_tobacco(void* arg) {
    while (1) {
        sem_wait(&smoker_tobacco_sem);

        printf("  -> Smoker with TOBACCO picks up items, makes, and smokes.\n");
        sleep(1); 

        sem_post(&agent_sem);
    }
    return NULL;
}

void* smoker_paper(void* arg) {
    while (1) {
        sem_wait(&smoker_paper_sem);

        printf("  -> Smoker with PAPER picks up items, makes, and smokes.\n");
        sleep(1); // Simulate smoking

        sem_post(&agent_sem);
    }
    return NULL;
}

void* smoker_matches(void* arg) {
    while (1) {
        sem_wait(&smoker_matches_sem);

        printf("  -> Smoker with MATCHES picks up items, makes, and smokes.\n");
        sleep(1); 

        sem_post(&agent_sem);
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    
    sem_init(&agent_sem, 0, 1);          
    sem_init(&smoker_tobacco_sem, 0, 0); 
    sem_init(&smoker_paper_sem, 0, 0);
    sem_init(&smoker_matches_sem, 0, 0);

    pthread_t agent_tid;
    pthread_t smoker_tids[3];

    printf("Starting Cigarette Smoker's Problem Simulation...\n");

    pthread_create(&agent_tid, NULL, agent_thread, NULL);
    pthread_create(&smoker_tids[0], NULL, smoker_tobacco, NULL);
    pthread_create(&smoker_tids[1], NULL, smoker_paper, NULL);
    pthread_create(&smoker_tids[2], NULL, smoker_matches, NULL);

    pthread_join(agent_tid, NULL);
    pthread_join(smoker_tids[0], NULL);
    pthread_join(smoker_tids[1], NULL);
    pthread_join(smoker_tids[2], NULL);

    sem_destroy(&agent_sem);
    sem_destroy(&smoker_tobacco_sem);
    sem_destroy(&smoker_paper_sem);
    sem_destroy(&smoker_matches_sem);

    return 0;

}
