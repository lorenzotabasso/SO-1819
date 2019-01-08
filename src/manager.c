#include "common.h"

int main (int argc, char * argv[]) {

    /* Init sim_parameters */
    init_msg_queue();
    read_conf("/Volumes/HDD/Lorenzo/Unito/2 Anno/SO/Progetto/Progetto 2018-2019/so-18-19/src/opt.conf");
    //start_timer();

    printf("PARENT (PID: %d): creating %d child processes\n", getpid(), POP_SIZE);

    for (int i = 0; i< POP_SIZE; i++) {
        switch (population[i] = fork()) {
            case -1:
                /* Handle error */
                print_error("Manager", errno);
                exit(EXIT_FAILURE);
            case 0:
                /* CHILD CODE */
                execve("src/student.c", argv, NULL);
                //printf("STUDENT (PID: %d): I'm Stud number %d, ADE_Mark: %d\n", getpid(), num_alive_procs, get_ade_mark());
                break;
            default:
                /* PARENT CODE */
                printf("PARENT (PID: %d): created child.\n", getpid());
                break;
        }
    }

    /* PARENT CODE: the child processes exited already */

    /* checking if any child proc terminated, stopped or continued */

   /* wait for any child process ("-1" = wait all)*/
//    while ((child_pid = waitpid(-1, &status, WUNTRACED | WCONTINUED)) != -1) {
//
//        printf("PARENT (PID: %d): Got info from child with PID=%d. Status 0x%04X\n", getpid(), child_pid, status);
//
//        /* Checking the status */
//        if (WIFEXITED(status)) {
//            /* the child proc exited, must decrement num_alive_procs */
//            printf("\tchild correctly exited with status %d\n", WEXITSTATUS(status));
//            num_alive_procs--;
//        }
//        if (WIFSIGNALED(status)) {
//            /* the child proc terminated by signal, must decrement num_alive_procs */
//            printf("\tchild terminated by the signal %d\n", WTERMSIG(status));
//            num_alive_procs--;
//        }
//        if (WIFSTOPPED(status)) {
//            printf("\tchild stopped by the signal %d\n", WSTOPSIG(status));
//        }
//        if (WIFCONTINUED(status)) {
//            printf("\tchild continued after being stopped\n");
//        }
//
//        printf("PARENT (PID: %d): \t\tKids left=%d\n", getpid(), num_alive_procs);
//    }

    fprintf(stdout, "PARENT (PID: %d): done with waiting because: %s (Err #%d)\n", getpid(), strerror(errno), errno);

    //signal(SIGALRM, stop_timer); // SIGALRM handler, it stops the timer.
    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}


