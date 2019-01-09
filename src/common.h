#ifndef COMMON_H
#define COMMON_H

#define POP_SIZE 1

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

int msg_queue_id;

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

void start_timer();
void stop_timer();

// It deallocates all the IPC allocated before.
void deallocate_IPCs();

#endif // COMMON_H
