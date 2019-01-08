#ifndef STUDENT_H
#define STUDENT_H

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

/* config settings */
int sim_time;
int dev_preference_2;
int dev_preference_3;
int dev_preference_4;
int max_reject;

void set_rand_ade_mark();
int get_ade_mark();

/* Utility */

void read_conf(char * conf_path);

void find_mates();

#endif /* STUDENT_H */
