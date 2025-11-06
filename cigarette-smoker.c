#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>     // For sleep()
#include <time.h>       // For time()

// --- Global Semaphores ---
// The agent waits on this, smokers signal it.
sem_t agent_sem;

// Each smoker waits on their specific "ingredient pair" semaphore.
sem_t smoker_tobacco_sem;
sem_t smoker_paper_sem;
sem_t smoker_matches_sem;

// For logging, to make output clearer
const char* items[] = {"TOBACCO", "PAPER", "MATCHES"};

/**
 * @brief The Agent Thread
 * Runs in a loop, placing two different ingredients on the table
 * and signaling the correct smoker.
 */
void* agent_thread(void* arg) {
    while (1) {
        // 1. Wait for a smoker to finish (or for the initial run)
        sem_wait(&agent_sem);

        // 2. Generate two *different* random ingredients (0, 1, or 2)
        int item1 = rand() % 3;
        int item2;
        do {
            item2 = rand() % 3;
        } while (item1 == item2);

        // 3. Place the items and signal the correct smoker
        // The items are 0=TOBACCO, 1=PAPER, 2=MATCHES

        if (item1 != 0 && item2 != 0) {
            // Items are PAPER (1) and MATCHES (2)
            printf("AGENT: Places %s and %s on the table.\n", items[1], items[2]);
            sem_post(&smoker_tobacco_sem); // Wake up smoker with TOBACCO
        } 
        else if (item1 != 1 && item2 != 1) {
            // Items are TOBACCO (0) and MATCHES (2)
            printf("AGENT: Places %s and %s on the table.\n", items[0], items[2]);
            sem_post(&smoker_paper_sem); // Wake up smoker with PAPER
        } 
        else {
            // Items are TOBACCO (0) and PAPER (1)
            printf("AGENT: Places %s and %s on the table.\n", items[0], items[1]);
            sem_post(&smoker_matches_sem); // Wake up smoker with MATCHES
        }
    }
    return NULL;
}

/**
 * @brief Smoker with TOBACCO
 * Waits for PAPER and MATCHES.
 */
void* smoker_tobacco(void* arg) {
    while (1) {
        // 1. Wait for the agent to signal that paper and matches are ready
        sem_wait(&smoker_tobacco_sem);

        // 2. Make and smoke
        printf("  -> Smoker with TOBACCO picks up items, makes, and smokes.\n");
        sleep(1); // Simulate smoking

        // 3. Signal the agent to provide the next items
        sem_post(&agent_sem);
    }
    return NULL;
}

/**
 * @brief Smoker with PAPER
 * Waits for TOBACCO and MATCHES.
 */
void* smoker_paper(void* arg) {
    while (1) {
        // 1. Wait for the agent to signal that tobacco and matches are ready
        sem_wait(&smoker_paper_sem);

        // 2. Make and smoke
        printf("  -> Smoker with PAPER picks up items, makes, and smokes.\n");
        sleep(1); // Simulate smoking

        // 3. Signal the agent
        sem_post(&agent_sem);
    }
    return NULL;
}

/**
 * @brief Smoker with MATCHES
 * Waits for TOBACCO and PAPER.
 */
void* smoker_matches(void* arg) {
    while (1) {
        // 1. Wait for the agent to signal that tobacco and paper are ready
        sem_wait(&smoker_matches_sem);

        // 2. Make and smoke
        printf("  -> Smoker with MATCHES picks up items, makes, and smokes.\n");
        sleep(1); // Simulate smoking

        // 3. Signal the agent
        sem_post(&agent_sem);
    }
    return NULL;
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Initialize semaphores
    // sem_init(sem_t *sem, int pshared, unsigned int value);
    // pshared = 0 means the semaphore is shared between threads of a process
    
    sem_init(&agent_sem, 0, 1);          // Agent starts at 1 (unlocked)
    sem_init(&smoker_tobacco_sem, 0, 0); // Smokers start at 0 (locked)
    sem_init(&smoker_paper_sem, 0, 0);
    sem_init(&smoker_matches_sem, 0, 0);

    // Create threads
    pthread_t agent_tid;
    pthread_t smoker_tids[3];

    printf("Starting Cigarette Smoker's Problem Simulation...\n");

    pthread_create(&agent_tid, NULL, agent_thread, NULL);
    pthread_create(&smoker_tids[0], NULL, smoker_tobacco, NULL);
    pthread_create(&smoker_tids[1], NULL, smoker_paper, NULL);
    pthread_create(&smoker_tids[2], NULL, smoker_matches, NULL);

    // Join threads (the program will run forever)
    pthread_join(agent_tid, NULL);
    pthread_join(smoker_tids[0], NULL);
    pthread_join(smoker_tids[1], NULL);
    pthread_join(smoker_tids[2], NULL);

    // Destroy semaphores (this code won't be reached, but good practice)
    sem_destroy(&agent_sem);
    sem_destroy(&smoker_tobacco_sem);
    sem_destroy(&smoker_paper_sem);
    sem_destroy(&smoker_matches_sem);

    return 0;
}