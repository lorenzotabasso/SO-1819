#include "common.h"

typedef struct {
    int id;
    int AdE_mark;
    int nof_elem;
    int ask;
    int nof_invites;
    int max_reject;
    int final_mark;
} student;

int get_random_mark(){
    srand((unsigned) getpid());
    return rand() % (30 + 1 - 18) + 18;
}

void search_mate(){

}

int num_alive_procs;

int main (int argc, char * argv[]) {
    int num_proc;
    pid_t child_pid, *all_kids;
    int status;

    /* If passed an argument to the executable */
    if (argc > 1) {
        /* If the argument is not a non-zero number */
        if (!(num_proc = atoi(argv[1]))) {
            num_proc = 8;
        }
    } else {
        num_proc = 4;
    }

    /* vector of kids PIDs */
    all_kids = malloc(num_proc * sizeof(*all_kids));

    printf("PARENT (PID=%d): creating %d child processes\n",
           getpid(), num_proc);
    for (num_alive_procs = 0; num_alive_procs < num_proc; num_alive_procs++) {
        switch (child_pid = fork()) {
            case -1:
                /* Handle error */
                exit(EXIT_FAILURE);
            case 0:
                /* CHILD CODE */
//                /* Free the array: not needed in child */
//                free(all_kids);
                printf("CHILD (PID=%d): id=%d, running stuff\n", getpid(), num_alive_procs);
                exit(num_alive_procs);
                break;
            default:
                /* PARENT CODE */
                printf("PARENT (PID=%d): created child (PID=%d)\n", getpid(), all_kids[num_alive_procs] = child_pid);
                break;
        }
    }

    /* PARENT CODE: the child processes exited already */

    /* checking if any child proc terminated, stopped or continued */

#if 1   /* wait for any child process */
    while ((child_pid = waitpid(-1, &status, WUNTRACED | WCONTINUED)) != -1) {
#else   /* wait for child processes from the latest created */
        while ((child_pid = waitpid(all_kids[num_alive_procs-1], &status, WUNTRACED | WCONTINUED)) != -1)
#endif

        printf("PARENT (PID=%d): Got info from child with PID=%d. Status 0x%04X\n", getpid(), child_pid, status);

        /* Checking the status */
        if (WIFEXITED(status)) {
            /* the child proc exited, must decrement num_alive_procs */
            printf("  child correctly exited with status %d\n", WEXITSTATUS(status));
            num_alive_procs--;
        }
        if (WIFSIGNALED(status)) {
            /* the child proc terminated by signal, must decrement num_alive_procs */
            printf("  child terminated by the signal %d\n", WTERMSIG(status));
#ifdef WCOREDUMP
            if (WCOREDUMP(status)) {
                printf("  child produced a core dump\n");
            }
#endif

            num_alive_procs--;
        }
        if (WIFSTOPPED(status)) {
            printf("  child stopped by the signal %d\n", WSTOPSIG(status));
        }
        if (WIFCONTINUED(status)) {
            printf("  child continued after being stopped\n");
        }

        printf("PARENT (PID=%d): Kids left=%d\n", getpid(), num_alive_procs);
    }

    fprintf(stderr, "PARENT (PID=%d): done with waiting because: %s (Err #%d)\n", getpid(), strerror(errno), errno);

    free(all_kids);
    exit(EXIT_SUCCESS);
}

// int main() {
//	pid_t my_pid, my_ppid;
//	int child_index = 0;
//    int * population = malloc(POP_SIZE * sizeof(student));
//
//	switch (fork()) {
//
//	    case -1:
//			/* Handle error */
//			print_error(errno);
//			break;
//
//		case 0:
//			/* Perform actions specific to child */
//
//            if (child_index <= POP_SIZE) {
//                my_pid = getpid();
//                my_ppid = getppid();
//                printf("CHILD-%d:  PID=%d, PPID=%d\n", child_index, my_pid, my_ppid);
//
//                population[child_index] = getpid();
//
//                child_index++;
//                break;
//            }
//            // signal "start" -> search_mate
//            printf("All child created"); // debug
//            break;
//
//		default:
//			/* Perform actions specific to parent */
//			my_pid = getpid();
//			my_ppid = getppid();
//			printf("PARENT: PID=%d, PPID=%d", my_pid, my_ppid);
//            printf("\nchild ended work\n");
//			break;
//		}
//		/* Both child and parent process will execute here!! */
//
//		printf("COMMON AREA PID: %d\n", getpid());
//
//        free(population);
//		exit(EXIT_SUCCESS);
// }


