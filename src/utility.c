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
        print_error("Manager, deallocate_IPCs", errno);
    }
//    if (semctl(semaforoPalla, 0, IPC_RMID) == -1){
//        Error();
//    }
//    if (semctl(setSemaforiSquadre, 1, IPC_RMID) == -1){
//        Error();
//    }
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
