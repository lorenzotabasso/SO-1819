#ifndef COMMON_H
#define COMMON_H

#define POP_SIZE 3
#define DEBUG 0

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

#define key_children_semaphore 1

int msg_queue_id;
int children_semaphore_id;

/* config settings */
int sim_time;
int dev_preference_2;
int dev_preference_3;
int dev_preference_4;
int nof_invites;
int max_reject;

/* vector of kids PIDs */
pid_t population[POP_SIZE];

/* student.c */
void set_rand_ade_mark();
void read_conf(char * conf_path);
void find_mates();
void handle_signal(int signal);

/* utility.c */
// It prints the error to stderr.
int print_error(char* from, int en);

// It returns a random number in the specified range.
// Seed parameter can be specified, otherwise,
// it's set to the utility process pid.
int random_between(pid_t seed, int min, int max);

// It initializes the common message queue for all the processes.
void init_msg_queue();
void init_children_semaphore (int semID);
int request_resource(int sem_id, int quantity);
void relase_resource(int sem_id, int quantity);

void start_timer();
void stop_timer();

// It deallocates all the IPC allocated before.
void deallocate_IPCs();

#endif // COMMON_H
