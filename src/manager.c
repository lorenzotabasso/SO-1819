#include "common.h"

int condition_print_avg = 0;

int main (int argc, char * argv[]) {

    // Init IPCs
    init_children_semaphore(KEY_CHILDREN_SEMAPHORE);
    id_rw_semaphore = semget(KEY_RW_SEMAPHORE, 1, IPC_CREAT | 0666);
    init_shared_memory(KEY_SHARED_MEMORY);

    id_message_queue_parent = msgget(KEY_MESSAGE_QUEUE_PARENT,IPC_CREAT | 0666);
    printf("id_message_queue_parent: %d\n", id_message_queue_parent);

    /* Init sim_parameters */
    read_conf(CONF_PATH);
    printf("PARENT (PID: %d) Configurazione caricata.\n", getpid());

    signal(SIGALRM, parent_handle_signal);

    printf("PARENT (PID: %d) Creo %d studenti\n", getpid(), POP_SIZE);
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
				printf("PARENT (PID %d) Creato studente (PID=%d)\n", getpid(), population[i]);
		}
	}

	/* PARENT CODE */

	/* after the creation of child, parent add POP_SIZE resource to the
	 * semaphore of children, in order to unblock them and to start the
	 * simulation.
	 */
    for(int j = 0; j < POP_SIZE; j++){
        relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set
    }

    /* Parent also put 1 resource on the RW semaphore, in order to handle the
     * race condition of all his children.*/
    relase_resource(id_rw_semaphore, 0);

    /* Parent initialize the matrix for the marsk in the shared memory. */
    set_shared_data();

    /* Parent start the timer for the simulation.*/
    start_timer();

	/* waiting for all child proceses */
    for (int i = 0; i < POP_SIZE; i++) {
        wait(0);
    }

    if (condition_print_avg == 1) {
        printf("\n"); // only for esthetics

        int fixed_mark = 0;
        int counter_mark = 0;
        int sum = 0;
        mark_list m_ade = create_node_mark(0);
        mark_list m_so = create_node_mark(-1);

        for (int i = 0; i < POP_SIZE; i++){
            fixed_mark = shm_pointer->marks[i][3];
            sum = sum + shm_pointer->marks[i][3];

            if (!contains_mark(m_ade, fixed_mark)){
                m_ade = insert_tail_mark(m_ade, fixed_mark);
                for (int j = 0; j < POP_SIZE; j++){

                    if(shm_pointer->marks[j][3] == fixed_mark) {
                        counter_mark++;
                    }
                }
                printf("PARENT (PID: %d) %d studenti hanno preso %d di AdE.\n", getpid(), counter_mark, fixed_mark);
                counter_mark = 0;
            }
        }
        fixed_mark = 0;
        printf("\nPARENT (PID: %d) la media dei vodi di AdE è %d\n\n", getpid(), sum / POP_SIZE);
        sum = 0;

        for (int i = 0; i < POP_SIZE; i++){
            fixed_mark = shm_pointer->marks[i][4];
            sum = sum + shm_pointer->marks[i][4];

            if (!contains_mark(m_so, fixed_mark)){
                m_so = insert_tail_mark(m_so, fixed_mark);
                for (int j = 0; j < POP_SIZE; j++){

                    if(shm_pointer->marks[j][4] == fixed_mark) {
                        counter_mark++;
                    }
                }
                printf("PARENT (PID: %d) %d studenti hanno preso %d di SO.\n", getpid(), counter_mark, fixed_mark);
                counter_mark = 0;
            }
        }
        fixed_mark = 0;
        printf("\nPARENT (PID: %d) la media dei voti di SO è %d\n\n", getpid(), sum / POP_SIZE);
        sum = 0;
    }

	printf("PARENT (PID %d) Ho finito di aspettare.\n", getpid());

    shmdt(shm_pointer); // Detaching shared memory

    deallocate_IPCs(); // Deallocating all IPCs (Parent and children)

    exit(EXIT_SUCCESS);
}

/* Parent initialize the matrix for the marks contained in the shared memory.
 * In particular, every row of the matrix contains: 
 *
 * marks[x][0] = the PID of the child, 
 * marks[x][1] = the group_id of the child,
 * marks[x][2] = the group_num of the child (which represent the number of 
 *               element in the child's group),
 * marks[x][3] = child's AdE mark,
 * marks[x][4] = child's SO Mark (initially set to 0)
 * marks[x][5] = child's pref_group (which represent the child 
 *               preference about the number of element of the group)
 *
 */
void set_shared_data(){
    shm_pointer = shmat(id_shared_memory, NULL, 0);
    if (shm_pointer == (void *) -1) {
        PRINT_ERROR;
    }

    for (int i = 0; i < POP_SIZE; i++) {
        shm_pointer->marks[i][0] = population[i];
        shm_pointer->marks[i][1] = 0;
        shm_pointer->marks[i][2] = 0;
        shm_pointer->marks[i][3] = 0;
        shm_pointer->marks[i][4] = 0; // default: the initial mark fo SO is 0
        shm_pointer->marks[i][5] = 0;
    }

    printf("PARENT (PID %d) Matrice in memoria condivisa inizializzata.\n", getpid());
}

/* Parent signal handler */
void parent_handle_signal(){
    kill(0,SIGCONT);

    compute_mark(POP_SIZE);

    sleep(2); // needed only for correctly displaying the marks on the terminal

    printf(YEL "\n\nPARENT (PID: %d) PUBBLICAZIONE VOTO APPELLO\n" RESET "\n", getpid());

    /* Releasing POP_SIZE resources on the children semaphore in order to 
     * unblock the children and finish simulation.
     */
    for(int j = 0; j < POP_SIZE; j++){
        relase_resource(id_children_semaphore, 0); // 0 -> the first semaphonre in the set
    }

    // Condition to print the fina lresult of the simulation
    condition_print_avg = 1;
}

/* It computes all the student's mark after sim_time expires.*/
void compute_mark(int number_marks){
    while(number_marks > 0){
        if (msgrcv(id_message_queue_parent,&costrutto2,sizeof(costrutto2),getpid(),0) == -1)
            PRINT_ERROR;
        printf(MAG "\tPARENT (PID: %d) ho ricevuto un messaggio dallo studente %d" RESET "\n", getpid(),costrutto2.sender);

        int found = 1;
        for (int i = 0; i < POP_SIZE && found; i++) {
            if (shm_pointer->marks[i][0] == costrutto2.student_msg) {
                shm_pointer->marks[i][1] = costrutto2.group_id_msg;
                shm_pointer->marks[i][2] = costrutto2.group_num_msg;
                shm_pointer->marks[i][3] = costrutto2.ade_mark_msg;
                shm_pointer->marks[i][5] = costrutto2.pref_gruppo_msg;

                found = 0;
            }
        }
        number_marks--; // exit condition of while
    }

    for (int i = 0; i < POP_SIZE; i++) {
        if (shm_pointer->marks[i][2] != 1) {
            int fixed_id;
            int max_mark = 0;

            // fix the group_id
            fixed_id = shm_pointer->marks[i][1];

            // finding the maximum mark in the group
            for (int j = 0; j < POP_SIZE; j++){
                if (shm_pointer->marks[j][1] == fixed_id) {
                    if (max_mark <= shm_pointer->marks[j][3]){
                        max_mark = shm_pointer->marks[j][3];
                    }
                }
            }

            // assigning max_mark to every group component
            for (int k = 0; k < POP_SIZE; k++){
                if (shm_pointer->marks[k][1] == fixed_id) {
                    if (shm_pointer->marks[k][2] != shm_pointer->marks[k][5]) {
                        shm_pointer->marks[k][4] = max_mark - 3;
                    } else {
                        shm_pointer->marks[k][4] = max_mark;
                    }
                }
            }
        }
    }
}
