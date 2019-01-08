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
        print_error("Student", errno);
    }
    printf("Created message queue with ID: %d\n", msg_queue_id);
}

void start_timer(int signal){
    if (signal == -1) {
        // the only process that can start and stop the timer
        print_error("Maganer", errno);
    } else {
        alarm(durataPartita);
        kill(getppid(), SIGUSR1);
    }
}

void stop_timer(int signal) {
    if (signal == -1) {
        Error();
    } else {
        fclose(fpL);
        sleep(1);
        printf("Tempo scaduto!\n\n");
        kill (arbitro, SIGALRM);
        pause();
    }
}

void deallocate_IPCs(){
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1){
        print_error("Student", errno);
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