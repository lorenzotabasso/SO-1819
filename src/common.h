#ifndef COMMON_H
#define COMMON_H

#define POP_SIZE 7
#define DEBUG {printf(RED "\t(%d) DEBUG at FILE: %s LINE:%d" RESET "\n", getpid(), __FILE__, __LINE__);}
#define PRINT_ERROR if (errno) {fprintf(stderr, "\t%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,    __LINE__, getpid(), errno, strerror(errno));}

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

#define CONF_PATH "src/opt.conf"

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

struct shared_memory {
    int marks[POP_SIZE][6]; // pid, id_gruppo, group_num, voto_ade, voto_so e pref_group
};

// originale
//int marks[POP_SIZE][2];

//int marks[POP_SIZE][4]; // pid, id_gruppo, pref_gruppo e voto

// pointer which will contain the memory
//address for the matrix of grouped processes
struct shared_memory * shm_pointer;

struct sigaction sa; // needed for signal hadling in student

typedef enum {FALSE,TRUE} bool;
typedef struct _node* list;

struct _node {
    //char ask; //stringa == il nostro dato
    int student;
    int voto_ade;
    int group_num;
    int pref_gruppo;
    list nxt; //puntatore al prossimo elemento
};

struct my_msg{
    long mtype;
    char ask;
    int ade_voto;
    int student_id;
    int pref_gruppo;
} costrutto,costrutto3;

// ORIGINALE
// struct my_msg2{
//     long mtype;
//     int sender;
//     int group_nums;
//     list gruppo;
// } costrutto2;

struct my_msg2{
    long mtype;
    int sender;
    int student_msg;
    int ade_mark_msg;
    int pref_gruppo_msg;
    int group_id_msg;
    int group_num_msg;
} costrutto2;

struct my_msg3{
    long mtype;
    int final_mark;
} mark;

#define KEY_CHILDREN_SEMAPHORE ftok("src/manager.c",1)
#define KEY_MESSAGE_QUEUE ftok("src/manager.c",2)
#define KEY_SHARED_MEMORY ftok("src/manager.c",3)
#define KEY_MESSAGE_QUEUE_PARENT ftok("src/manager.c",4)
#define KEY_MESSAGE_QUEUE_ANSWER ftok("src/manager.c",5)
#define KEY_RW_SEMAPHORE ftok("src/manager.c",6)

int id_children_semaphore;
int id_rw_semaphore;
int id_shared_memory;
int id_message_queue;
int id_message_queue_parent;
int id_message_queue_answer;
/* config settings */
int sim_time;
int dev_preference_2;
int dev_preference_3;
int dev_preference_4;
int nof_invites;
int max_reject;

/* vector of kids PIDs */
pid_t population[POP_SIZE];

/* manager.c */
void set_shared_data();
void parent_handle_signal();
void compute_mark(int number_marks);

/* student.c */
void invia_invito(); // invia inviti per unirsi al suo gruppo
void leggi_inviti(list l); // legge la list contenente gli inviti ricevuti
void init_student_parameters(); // inizializza le variabili  dello studente
void child_handle_signal();
void init_ipc_id();
void goodbye();

/* utility.c */
int random_between(pid_t seed, int min, int max);
void init_message_queue(int key_msg_queue);
void deallocate_msg_queue(int id_message_queue);
void init_children_semaphore (int key_sem);
int request_resource(int id_sem, int sem_num);
int relase_resource(int id_sem, int sem_num);
void init_shared_memory(int key_shmem);
void start_timer();
void stop_timer();
void deallocate_IPCs();
void read_conf(char * config_path);
void stampa_list (list l);
list crea_nodo (int i,int voto, int p);
list inserisci_in_testa(list l, int i,int voto,int p);
list inserisci_in_coda(list l,int i,int voto, int p);
list rimuovi_in_testa (list l);
list rimuovi_in_coda (list l);
list rimuovi_studente(list l, int n);
int contains(list l, int stud);
int calc_pref(int pref_2,int pref_3,int pref_4);

#endif // COMMON_H
