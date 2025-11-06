#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> // For rand()
#include <unistd.h>

// --- Constants and Globals ---

#define N 5 // 5 philosophers

// Philosopher states (from your 'enum')
#define THINKING 0
#define HUNGRY 1
#define EATING 2

// Macros to get left and right neighbors
// (i + N - 1) % N is a safe way to do (i - 1) for i=0
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N

// --- Shared Data (The "Monitor") ---
int state[N];           // Array of philosopher states
pthread_mutex_t mutex;  // Mutex for mutual exclusion (to enter the monitor)
pthread_cond_t self[N]; // Condition variables, one for each philosopher
// ------------------------------------

/*
 * This is the 'test' function from your image.
 * It checks if philosopher 'i' can eat.
 * It's only called from within pickup() or putdown(),
 * so the mutex is assumed to be *already locked*.
 */
void test(int i) {
    // If I'm hungry AND my neighbors are not eating
    if ((state[LEFT] != EATING) &&
        (state[i] == HUNGRY) &&
        (state[RIGHT] != EATING)) {
        // Set my state to EATING
        state[i] = EATING;
        printf("Philosopher %d is EATING\n", i);

        // Signal my condition variable 'self[i]'.
        // This wakes up philosopher 'i' if they were
        // waiting in the pickup() function.
        pthread_cond_signal(&self[i]);
    }
}

/*
 * This is the 'pickup' function from your image.
 * Called by a philosopher when they want to eat.
 */
void pickup(int i) {
    // --- Enter Monitor (Lock Mutex) ---
    pthread_mutex_lock(&mutex);

    // I am now hungry
    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);

    // Try to start eating
    test(i);

    // If test() failed (neighbors were eating), I must wait.
    // In pthreads (Mesa monitors), we MUST use a 'while' loop
    // to re-check the condition after waking up.
    // This is the C equivalent of "if (state[i] != EATING) self[i].wait;"
    while (state[i] != EATING) {
        // pthread_cond_wait atomically:
        // 1. Releases the 'mutex'
        // 2. Puts the thread to sleep (blocks)
        // 3. When signaled, it re-acquires the 'mutex' before waking up.
        pthread_cond_wait(&self[i], &mutex);
    }

    // --- Exit Monitor (Unlock Mutex) ---
    pthread_mutex_unlock(&mutex);
}

/*
 * This is the 'putdown' function from your image.
 * Called by a philosopher when they are done eating.
 */
void putdown(int i) {
    // --- Enter Monitor (Lock Mutex) ---
    pthread_mutex_lock(&mutex);

    // I am done eating, back to thinking
    state[i] = THINKING;
    printf("Philosopher %d is THINKING\n", i);

    // My forks are free.
    // Test if my left or right neighbors can eat now.
    test(LEFT);
    test(RIGHT);

    // --- Exit Monitor (Unlock Mutex) ---
    pthread_mutex_unlock(&mutex);
}

/*
 * This is the 'initialization_code' from your image.
 */
void init_monitor() {
    // Initialize the main mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialize the state and condition variable for each philosopher
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        pthread_cond_init(&self[i], NULL);
    }
}

/*
 * The main life-cycle of a philosopher thread.
 */
void *philosopher_life(void *num) {
    // Get my philosopher ID from the argument
    int i = *(int *)num;

    // Run forever
    while (1) {
        // --- Think ---
        // Simulate thinking for a random time
        sleep(rand() % 3 + 1);

        // --- Try to Eat ---
        pickup(i);

        // --- Eat ---
        // Simulate eating for a random time
        sleep(rand() % 3 + 1);

        // --- Finish Eating ---
        putdown(i);
    }
}

/*
 * Main function to start the simulation
 */
int main() {
    pthread_t threads[N];
    int philosopher_ids[N];

    // Call the initialization function
    init_monitor();

    printf("Starting the Dining Philosophers simulation...\n");

    // Create N philosopher threads
    for (int i = 0; i < N; i++) {
        philosopher_ids[i] = i; // Store the ID
        // Create the thread, passing it its ID
        if (pthread_create(&threads[i], NULL, philosopher_life, &philosopher_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to finish (which they won't, it's an infinite loop)
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}