#include "common.h"

int print_error(char* from, int en){
    fprintf(stderr,"%s: Error #%03d: %s\n", from, en, strerror(en));
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

void init_msg_queue(){
    msg_queue_id = msgget(IPC_PRIVATE, 0400 | 0200);
    if (msg_queue_id == -1) {
        print_error("Manager, init_msg_queue", errno);
    }
    printf("Created message queue with ID: %d\n", msg_queue_id);
}

void init_children_semaphore (int key_sem){
	children_semaphore_id = semget(key_sem, 1, IPC_CREAT | 0666);
	if (children_semaphore_id == -1) {
		print_error("Manager, initChildrenSemaphore - sem_ID", errno);
	}
	if (semctl(children_semaphore_id, 0, SETVAL, 0) == -1) {
		print_error("Manager, initChildrenSemaphore - SEMCTL", errno);
	}
	printf("Created Children Semaphore and initialized. child_sem_id=%d\n", children_semaphore_id);
}

int request_resource(int sem_id, int sem_num) {
    printf("requested resource\n");
    struct sembuf lock;
    lock.sem_num = sem_num;
    lock.sem_op=-1;
    lock.sem_flg=0;
    int ret = semop(sem_id, &lock, 1);
    
    if (ret == -1) 
        print_error("Error in request_resource", errno);
    else 
        return ret;
}

int relase_resource(int sem_id, int sem_num) {
   printf("release resource\n");
    struct sembuf unlock;
    unlock.sem_num = sem_num;
    unlock.sem_op=1;
    unlock.sem_flg=0;
    int ret = semop(sem_id, &unlock, 1);
    
    if (ret == -1) 
        print_error("Error in release_resource", errno);
    else 
        return ret;
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
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1){
        print_error("Manager, deallocate_IPCs - 1", errno);
    }
   if (semctl(children_semaphore_id, 0, IPC_RMID) == -1){
       print_error("Manager, deallocate_IPCs - 2", errno);
   }
//    if (shmctl(idshm, IPC_RMID, NULL) == -1) {
//        Error();
//    }
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
}
