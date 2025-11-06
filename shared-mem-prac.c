#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct SharedData {
    int arr[3];
};

int main() {
    struct SharedData *data;

    key_t key = 0x8746;

    int shmid = shmget(key, sizeof(struct SharedData), IPC_CREAT | 0666);

    data = (struct SharedData *)shmat(shmid, NULL, 0);

    pid_t pid = fork();

    if (pid == 0) {
        data->arr[0] = 56;
        printf("Child Arr 0: %d\n", data->arr[0]);
    } else {
        wait(NULL);
        data->arr[0] = 3;
        printf("Parent Arr 0: %d\n", data->arr[0]);
    }
}
