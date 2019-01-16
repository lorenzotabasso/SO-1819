#ifndef COMMON_H
#define COMMON_H

#define POP_SIZE 4

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#if defined(__linux__)
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short  *array;
        #if defined(__linux__)
            struct seminfo* __buf;
        #endif
    };
#endif

struct shared_data {
    int group_matrix[POP_SIZE][2];
};

struct message {
    long mtype;
    char mtext[5];
};

struct message msg_in_queue;

#define KEY_CHILDREN_SEM 1
#define KEY_SHARED_MEM 2

int id_children_semaphore;
int id_shared_memory;
int id_msg_queue;

/* config settings */
int sim_time;
int dev_preference_2;
int dev_preference_3;
int dev_preference_4;
int nof_invites;
int max_reject;

/* vector of kids PIDs */
pid_t population[POP_SIZE];

// pointer which will contain the memory 
//address for the matrix of grouped processes
struct shared_data * my_data;

#define TEST_ERROR    if (errno) {fprintf(stderr,			\
					  "\t%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));}

#define DEBUG    {printf("\t%s:%d: PID=%5d: Debug!\n", \
                      __FILE__,         \
                      __LINE__,         \
                      getpid());}

/* manager.c */
void set_shared_data();

/* student.c */
void set_rand_ade_mark();
void handle_signal(int signal);
void set_grouped(int condition);

/* utility.c */
int random_between(pid_t seed, int min, int max);
int select_random_receiver();
void init_msg_queue(int key_msg_queue);
int get_msg_queue_id(int id_queue);
void send_message(int id_queue, struct message to_send);
void deallocate_msg_queue(int id_msg_queue);
void init_children_semaphore (int key_sem);
int request_resource(int sem_id, int sem_num);
int relase_resource(int sem_id, int sem_num);
void init_shared_memory(int key_shmem);
void start_timer();
void stop_timer();
void read_conf(char * conf_path);
void deallocate_IPCs();

#endif // COMMON_H
