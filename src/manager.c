#include "common.h"

int process_voti = 0;

int main (int argc, char * argv[]) {
    //sa.sa_handler = test;
    //sa.sa_flags = SA_RESTART;

    // init IPCs
    init_children_semaphore(KEY_CHILDREN_SEMAPHORE);
    init_shared_memory(KEY_SHARED_MEMORY);

    //id_children_semaphore = semget(KEY_CHILDREN_SEMAPHORE,1,IPC_CREAT|0666);
    id_message_queue_parent = msgget(KEY_MESSAGE_QUEUE_PARENT,IPC_CREAT | 0666);
    printf("id_message_queue_parent: %d\n", id_message_queue_parent);

    /* Init sim_parameters */
    read_conf(CONF_PATH);

    signal(SIGALRM, test);

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

    start_timer();

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
        shm_pointer->marks[i][2] = 0;
        shm_pointer->marks[i][3] = 0;
        shm_pointer->marks[i][4] = 0; // default: il voto iniziale di SO è 0
        printf("[%d,%d,", shm_pointer->marks[i][0],shm_pointer->marks[i][1]);
        printf("%d,%d,", shm_pointer->marks[i][2],shm_pointer->marks[i][3]);
        printf("%d]\n", shm_pointer->marks[i][4]);
    }
    printf("PARENT (PID=%d): Shared memory matrix initialized.\n", getpid());
}

void test(){
    kill(0,SIGCONT);
    process_voti = POP_SIZE; // necesario, evita il student 0
    compute_mark(process_voti);
    for(int j = 0; j < POP_SIZE; j++){
        relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set
        printf(GRN "PARENT, PUTTING RES" RESET "\n");
    }
}

void compute_mark(int number_marks){
    while(number_marks > 0){
        if (msgrcv(id_message_queue_parent,&costrutto2,sizeof(costrutto2),getpid(),0) == -1)
            PRINT_ERROR;
        printf(MAG "\tPARENT (PID: %d) Received message from student %d" RESET "\n", getpid(),costrutto2.sender);

        // printf("%d\n", costrutto2.student_msg);
        // printf("%d\n", costrutto2.ade_mark_msg);
        // printf("%d\n", costrutto2.pref_gruppo_msg);
        // printf("%d\n", costrutto2.group_id_msg);
        // printf("%d\n", costrutto2.group_num_msg);

        int found = 1;
        for (int i = 0; i < POP_SIZE && found; i++) {
            if (shm_pointer->marks[i][0] == costrutto2.student_msg) {
                shm_pointer->marks[i][1] = costrutto2.group_id_msg;
                shm_pointer->marks[i][2] = costrutto2.group_num_msg;
                shm_pointer->marks[i][3] = costrutto2.ade_mark_msg;

                // printf("[%d,%d,", shm_pointer->marks[i][0],shm_pointer->marks[i][1]);
                // printf("%d,%d,", shm_pointer->marks[i][2],shm_pointer->marks[i][3]);
                // printf("%d]\n", shm_pointer->marks[i][4]);

                found = 0;
            }
        }
        number_marks--; // condizione uscita esterna

        // if (group_nums1 == 1) {
        //     int found = 1;
        //     for (int i = 0; i < POP_SIZE && found; i++) {
        //         if (marks[i][0] == gruppo1->student) {
        //             marks[i][1] = 0;
        //             found = 0;
        //         }
        //     }
        //     number_marks--; // condizione uscita esterna
        // } else {
        //     int max_mark = 0;

        //     troviamo il voto massimo
        //     while (gruppo1 != NULL) {
        //         if (max_mark <= gruppo1->voto_ade) {
        //             max_mark = gruppo1->voto_ade;
        //         }
        //         gruppo1 = gruppo1->nxt;
        //     }

        //     // togliamo i punti necessari
        //     while (gruppo1 != NULL) {
        //       int found = 1;
        //         // non è in gruppo con quanti voleva
        //         if (gruppo1->pref_gruppo != group_nums1) {
        //             for (int i = 0; i < POP_SIZE && found; i++) {
        //                 if (marks[i][0] == gruppo1->student) {
        //                     marks[i][1] = max_mark-3;
        //                     found = 0;
        //                 }
        //             }
        //         } else { // è in gruppo con quanti voleva
        //             for (int i = 0; i < POP_SIZE && found; i++) {
        //                 if (marks[i][0] == gruppo1->student) {
        //                     marks[i][1] = max_mark;
        //                     found = 0;
        //                 }
        //             }
        //         }
        //         number_marks--; // condizione uscita esterna
        //         gruppo1 = gruppo1->nxt;
        //     }
        // }
    }

    for (int i = 0; i < POP_SIZE; i++) {
        if (shm_pointer->marks[i][2] != 1) {
            int fixed_id;
            int max_mark = 0;

            fixed_id = shm_pointer->marks[i][1]; // fisso l'id del gruppo
            
            // trovo il voto massimo nel gruppo
            for (int j = 0; j < POP_SIZE; j++){
                if (shm_pointer->marks[j][1] == fixed_id) {
                    if (max_mark < shm_pointer->marks[j][3]){
                        max_mark = shm_pointer->marks[j][3];
                    }
                }
            }

            // assegno il voto a tutti i componenti
            for (int k = 0; k < POP_SIZE; k++){
                if (shm_pointer->marks[k][1] == fixed_id) {
                    shm_pointer->marks[k][4] = max_mark;
                }
            }
        }
    }

    // DEBUG;
    // while(number_marks > 0){
    //     DEBUG;
    //     msgrcv(id_message_queue_parent,&costrutto2,sizeof(costrutto2),0,0);
    //     DEBUG;

    //     int sender1 = costrutto2.sender;
    //     list gruppo1 = costrutto2.gruppo;
    //     int group_num1 = costrutto2.group_nums;

    //     // NON si toccano, servono per le computazioni
    //     int group_elem[group_num1][2];
    //     int max_mark = 0;

    //     // PROBLEMAAAA Dereferenziazione del puntatore/indirizzo
    //     //printf("TEST: %d\n", gruppo1->voto_ade);

    //     printf(MAG "\tPARENT (PID: %d) Received message from student %d" RESET "\n", getpid(),sender1);

    //     DEBUG;
    //     for (int i=0; i<group_num1 && gruppo1 != NULL; i++) {
    //         DEBUG;
    //         //stampa_list(gruppo1);

    //         group_elem[i][0] = gruppo1->student;
    //         if (max_mark <= gruppo1->voto_ade) {
    //             max_mark = gruppo1->voto_ade;
    //         }
    //         DEBUG;
    //         group_elem[i][1] = gruppo1->pref_gruppo;
    //         DEBUG;
    //         gruppo1 = gruppo1->nxt;
    //     }

    //     // TODO: fare una print dell'array group_elem;


    //     Da "VOTO DEL PROGETTO" nel testo:
    //     il voto di tutti gli studenti di un gruppo chiuso è determinato dal valore
    //     massimo del campo voto_AdE fra gli studenti del gruppo. A tale valore
    //     si sottraggono 3 punti nel caso in cui lo studente del gruppo si ritrovi a
    //     far parte di un gruppo che ha un numero di elementi diverso dal proprio
    //     obiettivo (specificato da nof_elems)

    //     DEBUG;
    //     for (int j=0; j<=group_num1 && group_elem[j][0] != 0; j++) {
    //         if (group_elem[j][1] == group_num1) {
    //             group_elem[j][1] = max_mark;
    //         } else {
    //             group_elem[j][1] = max_mark-3;
    //         }
    //     }

    //     DEBUG;
    //     // Assegnazione del voto allo studente in shared memory;
    //     int z = 0;
    //     int found = 1;
    //     while(z < group_num1) {
    //         for(int k = 0; k < POP_SIZE && found; k++) {
    //             if(group_elem[z][0] == shm_pointer->marks[k][0]){
    //                 shm_pointer->marks[k][1] = group_elem[z][1];
    //                 number_marks--; // CONDIZIONE USCITA WHILE ESTERNO
    //                 found = 0;
    //             }
    //         }
    //         z++;
    //     }
    //     DEBUG;
    // } // end while
    // DEBUG;
}
