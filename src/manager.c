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
                //execve("src/student.c", argv, NULL);
                printf("STUDENT (PID: %d): hi! I'm working!\n", getpid());
                exit(EXIT_SUCCESS);
                break;
            default:
                /* PARENT CODE */
                printf("PARENT (PID: %d): created child.\n", getpid());
                wait(0);
                break;
        }
    }

    /* PARENT CODE: the child processes exited already */

    /* checking if any child proc terminated, stopped or continued */

    fprintf(stdout, "PARENT (PID: %d): done with waiting\n", getpid());

    //signal(SIGALRM, stop_timer); // SIGALRM handler, it stops the timer.
    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}


