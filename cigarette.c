#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


sem_t agentLocker;

sem_t tobaccoSmoker;
sem_t paperSmoker;
sem_t matchesSmoker;

#define TOBACCO 0
#define PAPER 1
#define MATCHES 2

void agentHandler () {

	sem_wait(&agentLocker);

	srand(time);

	int thing1 = rand() % 3;
	int thing2;
	do {
		thing2 = rand() % 3;

	} while (thing1 == thing2);

	printf("The agent placed %s and %s", thing1, thing2);

}

int main () {

	sem_init(&agentLocker, 0, 1);

	sem_init(&tobaccoSmoker, 0, 0);
	sem_init(&paperSmoker, 0, 0);
	sem_init(&matchesSmoker, 0, 0);

	agentHandler();
}
