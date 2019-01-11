#include "common.h"

struct stud {
    int id;
    int ade_mark;
    int nof_elem;
    int ask;
    int nof_invites;
    int max_reject;
    int final_mark;
    int grouped;
} student;

struct sigaction sa;

void set_rand_ade_mark(){
    student.ade_mark = random_between(getpid(), 18, 30);
}

int main(int argc, char * argv[]) {
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;

    set_rand_ade_mark();
    printf("CHILD (PID: %d): Hi! I'm working! ADE_Mark: %d\n", getpid(), student.ade_mark);
    
    int test1 = request_resource(children_semaphore_id, 0);
    int test2 = semctl(children_semaphore_id, 0, GETVAL);
    
    if (test1 == -1) {
        print_error("TEST - 1", errno);
    } else if (test2 == -1){
        print_error("TEST - 2", errno);
    } else {
        printf("CHILD(%d)-REQUEST: %d\n", getpid(), test1);
        printf("CHILD(%d)-SEMCTL: %d\n", getpid(), test1);
    }

	exit(EXIT_SUCCESS);
}

void handle_signal(int signal) {
    printf("CHILD (PID: %d): got signal #%d: %s\n", getpid(), signal, strsignal(signal));
    
    switch (signal) {
        case SIGCONT:
            printf("CHILD (PID: %d): goodbye cruel word!\n", getpid());
            exit(EXIT_SUCCESS);
            
        default:
            break;
    }
}
