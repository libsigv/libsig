/*
 * A simple readers/writers program using a shared buffer and spinlocks.
 * (This file is based on 'read-write-2.c' from the user's images)
 */

#include <stdio.h>
#include <stdlib.h> // Added for exit()
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h> // Added for wait()
#include <unistd.h>

#define BUF_SIZE 5 /* logical size of buffer */
/* Total size is buffer + 'in' integer + 'out' integer */
#define SHARED_MEM_SIZE (BUF_SIZE + 2) * sizeof(int)
#define run_length 10 /* number of iterations in test run */

int main(void) {
    pid_t pid;           /* variable to record process id of child */
    void *shared_memory; /* shared memory base address (using void*) */

    int *in;     /* pointer to logical 'in' address for writer */
    int *out;    /* pointer to logical 'out' address for reader */
    int *buffer; /* logical base address for buffer */

    int i_child, j_child; /* index variables (j_child for parent) */
    int value;            /* value read by child */

    /* set up shared memory segment */
    shared_memory = mmap(0, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (shared_memory == MAP_FAILED) // Modern check for mmap failure
    {
        perror("error in mmap while allocating shared memory\n");
        exit(1);
    }

    /* * Shared memory segment is organized as:
     * [ buffer (BUF_SIZE ints) | 'in' (1 int) | 'out' (1 int) ]
     */

    buffer = (int *)shared_memory;
    /* Corrected pointer arithmetic:
       'in' is the int right after the buffer.
       'out' is the int right after 'in'.
       The image had a typo (multiplying by sizeof(int)
       in pointer arithmetic, which is incorrect).
    */
    in = (int *)shared_memory + BUF_SIZE;
    out = (int *)shared_memory + BUF_SIZE + 1;

    *in = *out = 0; /* initial starting points */

    if (-1 == (pid = fork())) /* check for error in spawning child process */
    {
        perror("error in fork");
        exit(1);
    }

    if (0 == pid) { /* processing for child == reader */
        printf("The reader process begins.\n");

        for (i_child = 0; i_child < run_length; i_child++) {
            while (*in == *out)
                ; /* spinlock waiting for data */
            value = buffer[*out];
            *out = (*out + 1) % BUF_SIZE;
            printf("Reader's report: item %2d == %2d\n", i_child, value);
        }
        printf("Reader done.\n");
    } else { /* processing for parent == writer */
        printf("The writer process begins.\n");

        for (j_child = 0; j_child < run_length; j_child++) {
            /* spinlock waiting for space */
            while ((*in + 1) % BUF_SIZE == *out)
                ;
            buffer[*in] = j_child; /* put data in buffer */
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