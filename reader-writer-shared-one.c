#include <stdio.h>
#include <stdlib.h> 
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

#define SIZE sizeof(int) /* size of [int] integer */
#define run_length 10    /* number of iterations in test run */

int main(void) {
    pid_t pid;            
    int *shared_memory;   
    int i_parent, i_child;
    int value;           

    shared_memory = (int *)mmap(0, SIZE, PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (shared_memory == (int *)-1) {
        perror("error in mmap while allocating shared memory\n");
        exit(1);
    }

    if ((pid = fork()) < 0) 
    {
        perror("error in fork");
        exit(1);
    }

    if (0 == pid) { 
        printf("The child process begins.\n");
        for (i_child = 0; i_child < run_length; i_child++) {
            sleep(1); 
            value = *shared_memory;
            printf("Child's report: current value = %2d\n", value);
        }
        printf("The child is done\n");
    } else { 
        printf("The parent process begins.\n");
        for (i_parent = 0; i_parent < run_length; i_parent++) {
            *shared_memory = i_parent * i_parent; 
            printf("Parent's report: current index = %2d\n", i_parent);
            sleep(1); 
        }
        wait(pid);
        printf("The parent is done\n");
    }

    munmap(shared_memory, SIZE);
    return 0;

}
