#include "manager.h"
#include "student.h"
#include "utility.h"

int main (int argc, char * argv[]) {
    int num_alive_procs;
    pid_t child_pid, * population;
    int status;
    student stud = {.ade_mark=0, .max_reject=0, .nof_invites=0, .ask=0, .id=0, .final_mark=0, .nof_elem=0};

    /* vector of kids PIDs */
    population = malloc(POP_SIZE * sizeof(* population));

    printf("PARENT (PID: %d): creating %d child processes\n", getpid(), POP_SIZE);

    for (num_alive_procs = 0; num_alive_procs < POP_SIZE; num_alive_procs++) {
        switch (child_pid = fork()) {
            case -1:
                /* Handle error */
                print_error(errno);
                exit(EXIT_FAILURE);
            case 0:
                /* CHILD CODE */
                set_rand_ade_mark(stud);
                printf("STUDENT (PID: %d): I'm Stud number %d, ADE_Mark: %d\n", getpid(), num_alive_procs, stud.ade_mark);

                exit(num_alive_procs);
                break;
            default:
                /* PARENT CODE */
                printf("PARENT (PID: %d): created child (PID: %d)\n", getpid(), population[num_alive_procs] = child_pid);
                break;
        }
    }

    // TODO: only for debug, to move in other line
    read_conf("/Volumes/HDD/Lorenzo/Unito/2 Anno/SO/Progetto/Progetto 2018-2019/so-18-19/src/opt.conf");

    /* PARENT CODE: the child processes exited already */

    /* checking if any child proc terminated, stopped or continued */

   /* wait for any child process ("-1")*/
    while ((child_pid = waitpid(-1, &status, WUNTRACED | WCONTINUED)) != -1) {

        printf("PARENT (PID: %d): Got info from child with PID=%d. Status 0x%04X\n", getpid(), child_pid, status);

        /* Checking the status */
        if (WIFEXITED(status)) {
            /* the child proc exited, must decrement num_alive_procs */
            printf("\tchild correctly exited with status %d\n", WEXITSTATUS(status));
            num_alive_procs--;
        }
        if (WIFSIGNALED(status)) {
            /* the child proc terminated by signal, must decrement num_alive_procs */
            printf("\tchild terminated by the signal %d\n", WTERMSIG(status));
            num_alive_procs--;
        }
        if (WIFSTOPPED(status)) {
            printf("\tchild stopped by the signal %d\n", WSTOPSIG(status));
        }
        if (WIFCONTINUED(status)) {
            printf("\tchild continued after being stopped\n");
        }

        printf("PARENT (PID: %d): \t\tKids left=%d\n", getpid(), num_alive_procs);
    }

    fprintf(stdout, "PARENT (PID: %d): done with waiting because: %s (Err #%d)\n", getpid(), strerror(errno), errno);

    free(population);
    exit(EXIT_SUCCESS);
}


