#include "common.h"

int main (int argc, char * argv[]) {
    int status;
    pid_t child_pid;
    //int num_alive_procs = POP_SIZE;

    /* Init sim_parameters */
    read_conf("src/opt.conf");

    // init IPCs
    init_children_semaphore(KEY_CHILDREN_SEM);
	init_shared_memory(KEY_SHARED_MEM);

    //start_timer();

    printf("PARENT (PID=%d): creating %d child processes\n", getpid(), POP_SIZE);
	for (int i=0; i < POP_SIZE; i++) {
		switch (population[i] = fork()) {
			case -1:
				/* Handle error */
				TEST_ERROR;
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

	/* after the creation of child, parent add POP_SIZE resource to the 
	 * semaphore of children, in order to unblock them and to start the 
	 * simulation
	 */
	for(int j = 0; j < POP_SIZE; j++)
		relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set

	// ATTENZIONE: NECESSARIO che questo frammento di codice della memoria 
	// condivisa stia QUI dopo il setting del semaforo, altrimenti si entra in un while eterno!
	set_shared_data();

	// waiting for all child proceses
	while ((child_pid = wait(&status)) > 0);
	
	printf("PARENT (PID=%d): done with waiting.\n", getpid());

    //signal(SIGALRM, stop_timer); // SIGALRM handler, it stops the timer.

    shmdt(my_data); // detaching shared memory

    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}

void set_shared_data(){
	my_data = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (my_data == (void *) -1) {
        TEST_ERROR;
    }

	/* initializing the matrix for the shared memory, in the first column of 
	 * the matrix will be set the PID of the processes, and in the second
	 * column will be set the group status (1 if the student is already 
	 * grouped, 0 otherwise)
	 */
	printf("PARENT (PID=%d): Initializing shared memory matrix.\n", getpid());
	for (int i = 0; i < POP_SIZE; i++) {
		my_data->group_matrix[i][0] = population[i];
		my_data->group_matrix[i][1] = 0;
	}
	printf("PARENT (PID=%d): Shared memory matrix initialized.\n", getpid());
}
