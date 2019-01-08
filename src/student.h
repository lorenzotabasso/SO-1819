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

typedef struct {
    int id;
    int ade_mark;
    int nof_elem;
    int ask;
    int nof_invites;
    int max_reject;
    int final_mark;
} student;

/* config settings */
int sim_time;
int dev_preference_2;
int dev_preference_3;
int dev_preference_4;
int max_reject;

void setup(student stud);

int get_ade_mark(student stud);

/* Getter and setters */
void set_id(student stud, int new_id);

void set_ade_mark(student stud, int new_ade_mark);

void set_rand_ade_mark(student stud);

void set_nof_elem(student stud, int new_nof_elem);

void set_ask(student stud, int new_ask);

void set_nof_invites(student stud, int new_nof_invites);

void set_max_reject(student stud, int new_max_reject);

/* Utility */

void read_conf(char * conf_path);

void find_mates();

#endif /* STUDENT_H */
