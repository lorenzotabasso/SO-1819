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

    // setting the IDs of children_semaphore and shared_memory in the child
    // (if we don't do that here, hoth semaphore and shared memory IDs will 
    // be = 0 and all the operation on sempahores will exit with an error).
    id_children_semaphore = semget(KEY_CHILDREN_SEM, 1, IPC_CREAT | 0666);
    id_shared_memory = shmget(KEY_SHARED_MEM, sizeof(*my_data), 0666 | IPC_CREAT);

    my_data = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (my_data == (void *) -1)
        TEST_ERROR;

    set_rand_ade_mark();
    printf("CHILD (PID: %d): Hi! I'm working! ADE_Mark: %d\n", getpid(), student.ade_mark);

    init_msg_queue(getpid());

    /* Child will remain blocked until the father will fill the semaphore 
     * with enougth resources to unlock them all.
     */
    request_resource(id_children_semaphore, 0);

    
    struct message msg;
    msg.mtext[1] = 'a';

    int pid_to_send;
    for (int i = 0; i <= POP_SIZE; ++i) {
        if (getpid() % 2 == 0){
            if (my_data->group_matrix[i][0] % 2 == 0) {
                pid_to_send = my_data->group_matrix[i][0];
            }
        } else {
            if (my_data->group_matrix[i][0] % 2 != 0) {
                pid_to_send = my_data->group_matrix[i][0];
            }
        }
    }

    // send_message(get_msg_queue_id(getpid()), msg);

    // DEBUG;
    // msgrcv(id_msg_queue, &msg_in_queue, sizeof(msg_in_queue)-sizeof(long), getpid(), 0);
    // switch(msg_in_queue.mtext[1]) {
    //     case 'a':
    //         DEBUG;
    //         printf("CHILD (PID: %d): message received!\n", getpid());
    //         break;
    //     default:
    //         DEBUG;
    //         break;
    // }
    // DEBUG;

    deallocate_msg_queue(id_msg_queue);

    shmdt(my_data); // detaching from shared memory

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
    for (int i = 0; i < POP_SIZE; i++) {
        int pid_to_test = my_data->group_matrix[i][0];
        if (pid_to_test == getpid()) {
            my_data->group_matrix[i][1] = condition;
        }
    }
}