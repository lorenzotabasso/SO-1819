#include "common.h"

int print_error(char* from, int en){
    fprintf(stderr,"\t[PID:%d]: %s: Error #%03d: %s\n", getpid(), from, en, strerror(en));
    return en;
}

int random_between(pid_t seed, int min, int max) {
    if (seed) {
        srand((unsigned) seed); // pid of the calling process
    } else {
        srand((unsigned) getpid()); // pid of this process
    }
    return rand() % (max + 1 - min) + min;
}

void init_msg_queue(int key_msg_queue){
    id_msg_queue = msgget(key_msg_queue, 0600 | IPC_CREAT);
    if (id_msg_queue == -1) {
        print_error("Error in init_msg_queue", errno);
    }
    printf("(PID:%d)Created message queue with ID: %d\n",getpid(), id_msg_queue);
}

int get_msg_queue_id(int id_queue){
    int ret = msgget(id_queue, 0600);
    if (ret == -1) {
        print_error("Error in get_msg_queue_id", errno);
    }
    return ret;
}

void send_message(int id_queue, struct message to_send) {
	if (msgsnd(id_queue, &to_send, sizeof(to_send)-sizeof(long), 0) == -1) {
		TEST_ERROR;
	}
}

void receive_message(int id_queue) {
	struct message my_msg;
	if (msgrcv(id_queue, &my_msg, sizeof(my_msg)-sizeof(long), 0, 0) == -1) {
		/* msgrcv failed!! */
		if (errno == EIDRM) {
			/* 
			 * La  coda  e` stata  cancellata  dal
			 * processo che ha trovato l'elenco di
			 * records pieno. Dobbiamo uscire.
			 */
			print_error("receive_message-1", errno);
		} else {
			/* Altri errori: inaspettato.
			 * Se accade dobbiamo approfondire
			 */
			TEST_ERROR;
			print_error("receive_message-2", errno);
		}
	}
	
	// File: test-msg-start e test-rcv-snd
}

void deallocate_msg_queue(int id_msg_queue){
    if (msgctl(id_msg_queue, IPC_RMID, NULL) == -1){
        print_error("Error in deallocate_msg_queue", errno);
    }
    printf("Message queue with ID: %d deallocated\n", id_msg_queue);
}

void init_children_semaphore (int key_sem){
	id_children_semaphore = semget(key_sem, 1, IPC_CREAT | 0666);
	if (id_children_semaphore == -1) {
		print_error("Manager, initChildrenSemaphore - ID_sem", errno);
	}
	if (semctl(id_children_semaphore, 0, SETVAL, 0) == -1) {
		print_error("Manager, initChildrenSemaphore - SEMCTL", errno);
	}
	printf("Created Children Semaphore and initialized. id_child_sem=%d\n", id_children_semaphore);
}

int request_resource(int id_sem, int sem_num) {
    printf("[%d]: requested resource\n", getpid());
    struct sembuf lock;
    lock.sem_num = sem_num;
    lock.sem_op=-1;
    lock.sem_flg=0;
    int ret = semop(id_sem, &lock, 1);
    
    if (ret == -1) 
        print_error("Error in request_resource", errno);
    else 
        return ret;
}

int relase_resource(int id_sem, int sem_num) {
   printf("[%d]: release resource\n", getpid());
    struct sembuf unlock;
    unlock.sem_num = sem_num;
    unlock.sem_op=1;
    unlock.sem_flg=0;
    int ret = semop(id_sem, &unlock, 1);
    
    if (ret == -1) 
        print_error("Error in release_resource", errno);
    else 
        return ret;
}

void init_shared_memory(int key_shmem) {
    id_shared_memory = shmget(key_shmem, sizeof(*my_data), 0666 | IPC_CREAT);
    if (id_shared_memory == -1) {
        print_error("Manager, init_shared_memory", errno);
    }
    printf("Shared memory initialized with ID: %d\n", id_shared_memory);
}

void start_timer(){
    alarm((unsigned int) sim_time);
}

void stop_timer() {
    sleep(1);
    printf("Timeout!\n\n");
    kill(0, SIGCONT); // kill everyone
}

void deallocate_IPCs(){
    // if (msgctl(id_msg_queue, IPC_RMID, NULL) == -1){
    //     print_error("Manager, deallocate_IPCs - 1", errno);
    // }
    if (semctl(id_children_semaphore, 0, IPC_RMID) == -1){
        print_error("Manager, deallocate_IPCs - 2", errno);
    }
    if (shmctl(id_shared_memory, IPC_RMID, NULL) == -1) {
        print_error("Manager, deallocate_IPCs - 3", errno);
    }
    printf ("IPCs deallocated successfully.\n");
}

void read_conf(char * config_path){
    int max = 100;
    char line[max];
    FILE *config_fp = fopen(config_path, "r");

    if (config_fp == NULL){
        print_error("Manager, read_conf", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "sim_time = %d", &sim_time) != 1) {
        print_error("Manager, read_conf", errno);
    } else {
        if (sim_time < 0)
            print_error("Manager, read_conf", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_2 = %d", &dev_preference_2) != 1) {
        print_error("Manager, read_conf", errno);
    } else {
        if (sim_time < 0)
            print_error("Manager, read_conf", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_3 = %d", &dev_preference_3) != 1) {
        print_error("Manager, read_conf", errno);
    } else {
        if (sim_time < 0)
            print_error("Manager, read_conf", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_4 = %d", &dev_preference_4) != 1) {
        print_error("Manager, read_conf", errno);
    } else {
        if (sim_time < 0)
            print_error("Manager, read_conf", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "nof_invites = %d", &nof_invites) != 1) {
        print_error("Manager, read_conf", errno);
    } else {
        if (sim_time < 0)
            print_error("Manager, read_conf", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "max_reject = %d", &max_reject) != 1) {
        print_error("Manager, read_conf", errno);
    } else {
        if (sim_time < 0)
            print_error("Manager, read_conf", errno);
    }
    printf("Config loaded.\n");
}
