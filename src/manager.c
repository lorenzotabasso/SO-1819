#include "common.h"

int main (int argc, char * argv[]) {

    /* Init sim_parameters */
    init_msg_queue();
    read_conf("src/opt.conf");
    //start_timer();

    printf("PARENT (PID: %d): creating %d child processes\n", getpid(), POP_SIZE);

    for (int i = 0; i< POP_SIZE; i++) {
        switch (population[i] = fork()) {
            case -1:
                /* Handle error */
                print_error("Manager, Creating child processes", errno);
                exit(EXIT_FAILURE);
            case 0:
                /* CHILD CODE */
                execve("student", argv, NULL);
                //exit(EXIT_SUCCESS);
                
            default:
                /* PARENT CODE */
                printf("PARENT (PID: %d): created child.\n", getpid());
        }
    }

    /* PARENT CODE: the child processes exited already */
    /* checking if any child proc terminated, stopped or continued */

    // wakeup all child
    for(int i = 0; i < POP_SIZE; i++){
        printf("[%d]=%d\t", i, population[i]);
        kill(population[i], SIGCONT);
    }
    printf("\n");

    // for(int i = 0; i < POP_SIZE; i++) {
    //     wait(0);
    // }

    printf("PARENT (PID: %d): done with waiting\n", getpid());

    //signal(SIGALRM, stop_timer); // SIGALRM handler, it stops the timer.
    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}
