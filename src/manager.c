#include "common.h"

int main (int argc, char * argv[]) {
    int status;
    pid_t child_pid;
    
    int process_voti = POP_SIZE;

    // init IPCs
    init_children_semaphore(KEY_CHILDREN_SEMAPHORE);
    init_shared_memory(KEY_SHARED_MEMORY);

    id_children_semaphore = semget(KEY_CHILDREN_SEMAPHORE,1,IPC_CREAT|0666);
    id_message_queue = msgget(KEY_MESSAGE_QUEUE,IPC_CREAT | 0666);
    
    /* Init sim_parameters */
    read_conf("src/opt.conf");
    init_sem_zero(id_children_semaphore,0);
    //start_timer();

    printf("PARENT (PID=%d): creating %d child processes\n", getpid(), POP_SIZE);
	for (int i=0; i < POP_SIZE; i++) {
		switch (population[i] = fork()) {
			case -1:
				/* Handle error */
				PRINT_ERROR;
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
	for(int j = 0; j < POP_SIZE*2; j++) // *2 per via del "doppio blocco" del figlio
		relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set

    while(process_voti > 0){
        msgrcv(id_message_queue,&costrutto2,sizeof(costrutto2),getpid(),0666);
        int i = 0;
        int group_elem[3][1];
        int max_mark = 0;

        while (costrutto2.gruppo->nxt != NULL) {
            group_elem[i][0] = costrutto2.gruppo->student;
            if (max_mark <= costrutto2.gruppo->voto_ade) {
                max_mark = costrutto2.gruppo->voto_ade;
            }
            group_elem[i][1] = costrutto2.gruppo->pref_gruppo;
            i++;
        }

        for (int j=0; j<=3 && group_elem[j][0] != 0; j++) {
            if (group_elem[j][1] == i) {
                group_elem[j][1] = max_mark;
            } else {
                group_elem[j][1] = max_mark-3;
            }
        }

        int found;
        int i1 = 0;
        for (int k = 0; k < POP_SIZE; k++) {
            found = 1;
            while(found) {
                if(group_elem[i1][0] == my_shm->marks[i1][0]){
                    my_shm->marks[i1][1] = group_elem[i1][1];
                    found = 0;
                    process_voti--; // CONDIZIONE USCITA WHILE ESTERNO
                }
                i1++;
            }
        }
    }

	// ATTENZIONE: NECESSARIO che questo frammento di codice della memoria 
	// condivisa stia QUI dopo il setting del semaforo, altrimenti si entra in un while eterno!
	set_shared_data();

	// waiting for all child proceses
	while ((child_pid = wait(&status)) > 0);
	
	printf("PARENT (PID=%d): done with waiting.\n", getpid());

    //signal(SIGALRM, stop_timer); // SIGALRM handler, it stops the timer.

    shmdt(my_shm); // detaching shared memory

    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}

void set_shared_data(){
    my_shm = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (my_shm == (void *) -1) {
        PRINT_ERROR;
    }

    /* initializing the matrix for the shared memory, in the first column of
     * the matrix will be set the PID of the processes, and in the second
     * column will be set the group status (1 if the student is already
     * grouped, 0 otherwise)
     */
    printf("PARENT (PID=%d): Initializing shared memory matrix.\n", getpid());
    for (int i = 0; i < POP_SIZE; i++) {
        my_shm->marks[i][0] = population[i];
        my_shm->marks[i][1] = 0;
    }
    printf("PARENT (PID=%d): Shared memory matrix initialized.\n", getpid());
}
