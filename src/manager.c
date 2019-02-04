#include "common.h"

int main (int argc, char * argv[]) {
    int status;
    pid_t child_pid;

    int process_voti = POP_SIZE;

    // init IPCs
    init_children_semaphore(KEY_CHILDREN_SEMAPHORE);
    init_shared_memory(KEY_SHARED_MEMORY);

    //id_children_semaphore = semget(KEY_CHILDREN_SEMAPHORE,1,IPC_CREAT|0666);
    id_message_queue = msgget(KEY_MESSAGE_QUEUE,IPC_CREAT | 0666);
    printf("\tMSG %d\n", id_message_queue);

    /* Init sim_parameters */
    read_conf("src/opt.conf");

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
     for(int j = 0; j < POP_SIZE; j++){
        relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set
        printf(GRN "PARENT, PUTTING RES" RESET "\n");
    }
    // ATTENZIONE: NECESSARIO che questo frammento di codice della memoria
    // condivisa stia QUI dopo il setting del semaforo, altrimenti si entra in un while eterno!
    set_shared_data();

    //start_timer(3);

    PRINT_ERROR;

    DEBUG;
    while(process_voti > 0){
        DEBUG;
        msgrcv(id_message_queue,&costrutto2,sizeof(costrutto2),getpid(),0666);
        printf("MSGQUEUEID: %d", id_message_queue);
        DEBUG;
        PRINT_ERROR;
        int group_num = costrutto2.group_num;
        int group_elem[group_num][2];
        int max_mark = 0;
        DEBUG;

        for (int i=0; i<group_num; i++) {
            DEBUG;
            int uno = costrutto2.gruppo->student;
            group_elem[i][0] = uno;
            DEBUG;
            if (max_mark <= costrutto2.gruppo->voto_ade) {
                max_mark = costrutto2.gruppo->voto_ade;
            }
            group_elem[i][1] = costrutto2.gruppo->pref_gruppo;
        }
        DEBUG;

        /*
        Da "VOTO DEL PROGETTO" nel testo:
        il voto di tutti gli studenti di un gruppo chiuso è determinato dal valore
        massimo del campo voto_AdE fra gli studenti del gruppo. A tale valore
        si sottraggono 3 punti nel caso in cui lo studente del gruppo si ritrovi a
        far parte di un gruppo che ha un numero di elementi diverso dal proprio
        obiettivo (specificato da nof_elems)
        */

        for (int j=0; j<=group_num && group_elem[j][0] != 0; j++) {
            if (group_elem[j][1] == group_num) {
                group_elem[j][1] = max_mark;
            } else {
                group_elem[j][1] = max_mark-3;
            }
        }
        DEBUG;

        // Assegnazione del voto allo studente in shared memory;
        int z = 0;
        int found = 1;
        while(z<group_num ) {
            for(int k = 0; k < POP_SIZE && found; k++) {
                if(group_elem[z][0] == shm_pointer->marks[k][0]){
                    shm_pointer->marks[k][1] = group_elem[z][1];
                    process_voti--; // CONDIZIONE USCITA WHILE ESTERNO
                    found = 0;
                }
            }
            z++;
        }
    }
    DEBUG;

    //for(int j = 0; j < POP_SIZE; j++) // *2 per via del "doppio blocco" del figlio
    //    relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set

	// waiting for all child proceses
	//while ((child_pid = wait(&status)) > 0);
    for (int i = 0; i < POP_SIZE; i++) {
        wait(0);
    }

	printf("PARENT (PID=%d): done with waiting.\n", getpid());

    //shmdt(shm_pointer); // detaching shared memory

    deallocate_IPCs();

    exit(EXIT_SUCCESS);
}

void set_shared_data(){
    shm_pointer = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (shm_pointer == (void *) -1) {
        PRINT_ERROR;
    }

    /* initializing the matrix for the shared memory, in the first column of
     * the matrix will be set the PID of the processes, and in the second
     * column will be set the group status (1 if the student is already
     * grouped, 0 otherwise)
     */
    printf("PARENT (PID=%d): Initializing shared memory matrix.\n", getpid());
    for (int i = 0; i < POP_SIZE; i++) {
        shm_pointer->marks[i][0] = population[i];
        shm_pointer->marks[i][1] = 0;
        printf("[%d,", shm_pointer->marks[i][0]);
        printf("%d]\n", shm_pointer->marks[i][1]);
    }
    printf("PARENT (PID=%d): Shared memory matrix initialized.\n", getpid());
}
