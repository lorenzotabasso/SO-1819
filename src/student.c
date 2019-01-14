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

    // setting the children_semaphore_id in the child
    // (if we don't do that here, children_semaphore_id will be = 0 and all
    // the operation on sempahores will exit with an error).
    id_children_semaphore = semget(KEY_CHILDREN_SEM, 1, IPC_CREAT | 0666);
    id_shared_memory = shmget(KEY_SHARED_MEM, sizeof(*my_data), 0666 | IPC_CREAT);

   printf("CHILD: shared memory id: %d\n", id_shared_memory);
    my_data = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (my_data == (void *) -1)
        print_error("Student, attaching shared memory", errno);

    printf("Detaching shmem: %d\n", shmdt(my_data));

    set_rand_ade_mark();
    printf("CHILD (PID: %d): Hi! I'm working! ADE_Mark: %d\n", getpid(), student.ade_mark);

    int test1 = request_resource(id_children_semaphore, 0);
    int test2 = semctl(id_children_semaphore, 0, GETVAL);
    
    if (test1 == -1) {
        print_error("TEST - 1", errno);
    } else if (test2 == -1){
        print_error("TEST - 2", errno);
    } else {
        printf("CHILD(%d)-REQUEST: %d\n", getpid(), test1);
        printf("CHILD(%d)-SEMCTL: %d\n", getpid(), test1);
    }

    printf("CHILD (PID: %d): exiting!\n", getpid());

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

void set_grouped(int condition) {
    
}