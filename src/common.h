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

/* student.c */
int get_random_mark();

/* utility.c */
int msg_queue_id;

// It prints the error to stderr.
int print_error(int en);

// It initializes the common message queue for all the processes.
void init_msg_queue();

// It deallocates all the IPC allocated before.
void deallocate_IPCs();

#endif // COMMON_H
