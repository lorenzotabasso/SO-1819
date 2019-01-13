#include "common.h"

int main (int argc, char * argv[]) {
    int status;
    pid_t child_pid;
    int num_alive_procs = POP_SIZE;
    struct shared_data * my_data; 	// pointer which will contain the memory 
    								//address for the matrix of grouped processes

    /* Init sim_parameters */
    read_conf("src/opt.conf");
    init_children_semaphore(key_children_semaphore);
    init_msg_queue();

	init_shared_memory(key_shared_memory, my_data);
	my_data = shmat(shared_memory_id, NULL, 0);

    //start_timer();

    printf("PARENT (PID=%d): creating %d child processes\n", getpid(), POP_SIZE);
	for (int i=0; i < POP_SIZE; i++) {
		switch (population[i] = fork()) {
			case -1:
				/* Handle error */
				print_error("manager, creating children", errno);
				exit(EXIT_FAILURE);
			case 0:
				/* CHILD CODE */
				execve("bin/student", argv, NULL);
				
			default:
				/* PARENT CODE */
				printf("PARENT (PID=%d): created child (PID=%d)\n", getpid(), population[i]);
		}
	}
		
	/* PARENT CODE: the child processes exited already */
	
	/* initializing the matrix for the shared memory, in the first column of 
	 * the matrix will be set the PID of the processes, and in the second
	 * column will be set the group status (1 if the student is already 
	 * grouped, 0 otherwise)
	 */
	for (int i = 0; i < POP_SIZE; i++) {
		my_data->group_matrix[i][0] = population[i];
		my_data->group_matrix[i][1] = 0;
	}

	/* after the creation of child, parent add POP_SIZE resource to the 
	 * semaphore of children, in order to unblock them and to start the 
	 * simulation
	 */
	for(int j = 0; j < POP_SIZE; j++)
		relase_resource(children_semaphore_id, 0); // 0 -> the first semaphonre in the set

	/* checking if any child proc terminated, stopped or continued */
   /* wait for any child process */
	while ((child_pid = waitpid(-1,&status,WUNTRACED | WCONTINUED)) != -1) {
		printf("PARENT (PID=%d): Got info from child with PID=%d. Status 0x%04X\n", getpid(), child_pid, status);
		
        /* Checking the status */
		if (WIFEXITED(status)) {
			/* the child proc exited, must decrement num_alive_procs */
			printf("  child correctly exited with status %d\n", WEXITSTATUS(status));
			num_alive_procs--;
		}
		if (WIFSIGNALED(status)) {
			/* the child proc terminated by signal, must decrement num_alive_procs */
			printf("  child terminated by the signal %d\n",WTERMSIG(status));
			num_alive_procs--;
		}
		if (WIFSTOPPED(status)) {
			printf("  child stopped by the signal %d\n", WSTOPSIG(status));
		}
		if (WIFCONTINUED(status)) {
			printf("  child continued after being stopped\n");
		}
		printf("PARENT (PID=%d): Kids left=%d\n",getpid(), num_alive_procs);
	}
	
	fprintf(stderr, "PARENT (PID=%d): done with waiting because: %s (Err #%d)\n", getpid(), strerror(errno), errno);

    //signal(SIGALRM, stop_timer); // SIGALRM handler, it stops the timer.
    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}
