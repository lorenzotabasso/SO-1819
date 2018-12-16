#ifndef SO_18_19_COMMON_H
#define SO_18_19_COMMON_H

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

/* Utility.c */
int msg_queue_id;

void print_error(int en);
void init_msg_queue();
void deallocate_IPCs();

#endif //SO_18_19_COMMON_H
