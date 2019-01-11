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

#define TEST_ERROR    if (errno) {fprintf(stderr,			\
					  "%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));}

/* student.c */
void set_rand_ade_mark();
void handle_signal(int signal);

/* utility.c */
int print_error(char* from, int en);
int random_between(pid_t seed, int min, int max);
void init_msg_queue();
void init_children_semaphore (int key_sem);
int request_resource(int sem_id, int sem_num);
int relase_resource(int sem_id, int sem_num);
void start_timer();
void stop_timer();
void read_conf(char * conf_path);
void deallocate_IPCs();

#endif // COMMON_H
