
#include <stdio.h>
#include <stdlib.h> 
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

#define BUF_SIZE 5
#define SHARED_MEM_SIZE (BUF_SIZE + 2) * sizeof(int)
#define run_length 10

int main(void) {
    pid_t pid;          
    void *shared_memory;

    int *in;     
    int *out;   
    int *buffer; 

    int i_child, j_child;
    int value;           

    shared_memory = mmap(0, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (shared_memory == MAP_FAILED) 
    {
        perror("error in mmap while allocating shared memory\n");
        exit(1);
    }


    buffer = (int *)shared_memory;
    in = (int *)shared_memory + BUF_SIZE;
    out = (int *)shared_memory + BUF_SIZE + 1;

    *in = *out = 0; 

    if (-1 == (pid = fork())) 
    {
        perror("error in fork");
        exit(1);
    }

    if (0 == pid) {
        printf("The reader process begins.\n");

        for (i_child = 0; i_child < run_length; i_child++) {
            while (*in == *out)
                ;
            value = buffer[*out];
            *out = (*out + 1) % BUF_SIZE;
            printf("Reader's report: item %2d == %2d\n", i_child, value);
        }
        printf("Reader done.\n");
    } else { 
        printf("The writer process begins.\n");

        for (j_child = 0; j_child < run_length; j_child++) {
            while ((*in + 1) % BUF_SIZE == *out)
                ;
            buffer[*in] = j_child; 
            *in = (*in + 1) % BUF_SIZE;
            printf("Writer's report: item %2d put in buffer\n", j_child);
        }
        wait(pid);
        printf("Writer done.\n");
        exit(0); // Parent exits
    }

    /* * Note: The child process also needs to exit.
     * The mmap cleanup (munmap) is also missing,
     * but this matches the provided code.
     */
    return 0; // Only the child process will reach this
}
