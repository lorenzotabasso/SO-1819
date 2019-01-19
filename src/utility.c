#include "common.h"

int random_between(pid_t seed, int min, int max) {
    if (seed) {
        srand((unsigned) seed); // pid of the calling process
    } else {
        srand((unsigned) getpid()); // pid of this process
    }
    return rand() % (max + 1 - min) + min;
}

void init_message_queue(int key_msg_queue){
    id_message_queue = msgget(key_msg_queue, 0666 | IPC_CREAT);
    if (id_msg_queue == -1) {
        PRINT_ERROR;
    }
    printf("(PID:%d)Created message queue with ID: %d\n",getpid(), id_msg_queue);
}

// int get_msg_queue_id(int id_queue){
//     int ret = msgget(id_queue, 0666);
//     if (ret == -1) {
//         TEST_ERROR;
//     }
//     return ret;
// }

void send_message(int id_queue, struct message to_send) {
	if (msgsnd(id_queue, &to_send, sizeof(to_send)-sizeof(long), 0) == -1) {
		TEST_ERROR;
	}
}

void deallocate_msg_queue(int id_msg_queue){
    if (msgctl(id_msg_queue, IPC_RMID, NULL) == -1){
        TEST_ERROR;
    }
    printf("Message queue with ID: %d deallocated\n", id_msg_queue);
}

void init_children_semaphore (int key_sem){
	id_children_semaphore = semget(key_sem, 1, IPC_CREAT | 0666);
	if (id_children_semaphore == -1) {
		TEST_ERROR;
	}
	if (semctl(id_children_semaphore, 0, SETVAL, 0) == -1) {
		TEST_ERROR;
	}
	printf("Created Children Semaphore and initialized. id_child_sem=%d\n", id_children_semaphore);
}

int request_resource(int id_sem, int sem_num) {
    struct sembuf lock;
    lock.sem_num = sem_num;
    lock.sem_op=-1;
    lock.sem_flg=0;
    int ret = semop(id_sem, &lock, 1);
    
    if (ret == -1) {
        PRINT_ERROR;
    }
    return ret;
}

int relase_resource(int id_sem, int sem_num) {
    struct sembuf unlock;
    unlock.sem_num = sem_num;
    unlock.sem_op=1;
    unlock.sem_flg=0;
    int ret = semop(id_sem, &unlock, 1);
    
    if (ret == -1){
        PRINT_ERROR;
    }
    return ret;
}

void init_shared_memory(int key_shmem) {
    id_shared_memory = shmget(key_shmem, sizeof(*my_shm), 0666 | IPC_CREAT);
    if (id_shared_memory == -1) {
        PRINT_ERROR;
    }
    printf("Shared memory initialized with ID: %d\n", id_shared_memory);
}

void start_timer(){
    alarm((unsigned int) sim_time);
}

void stop_timer() {
    sleep(1);
    printf("Timeout!\n\n");
    kill(0, SIGCONT); // kill everyone
}

void deallocate_IPCs(){
    for (int i = 0; i < POP_SIZE; i++) {
        deallocate_msg_queue(get_msg_queue_id(population[i]));
    }
    if (semctl(id_children_semaphore, 0, IPC_RMID) == -1){
        TEST_ERROR;
    }
    if (shmctl(id_shared_memory, IPC_RMID, NULL) == -1) {
        TEST_ERROR;
    }
    printf ("IPCs deallocated successfully.\n");
}

void read_conf(char * config_path){
    int max = 100;
    char line[max];
    FILE *config_fp = fopen(config_path, "r");

    if (config_fp == NULL){
        TEST_ERROR;
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "sim_time = %d", &sim_time) != 1) {
        TEST_ERROR;
    } else {
        if (sim_time < 0)
            TEST_ERROR;
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_2 = %d", &dev_preference_2) != 1) {
        TEST_ERROR;
    } else {
        if (sim_time < 0)
            TEST_ERROR;
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_3 = %d", &dev_preference_3) != 1) {
        TEST_ERROR;
    } else {
        if (sim_time < 0)
            TEST_ERROR;
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_4 = %d", &dev_preference_4) != 1) {
        TEST_ERROR;
    } else {
        if (sim_time < 0)
            TEST_ERROR;
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "nof_invites = %d", &nof_invites) != 1) {
        TEST_ERROR;
    } else {
        if (sim_time < 0)
            TEST_ERROR;
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "max_reject = %d", &max_reject) != 1) {
        TEST_ERROR;
    } else {
        if (sim_time < 0)
            TEST_ERROR;
    }
    printf("Config loaded.\n");
}

// UTILITY su srutture dati ---------------------------------------------------

void stampa_lista (lista l){
    while (l != NULL){
        printf("%d; ", (*l).student);
        l=(*l).nxt;
    }
    printf("\n");
}

lista crea_nodo (int i,int voto, int p){
    lista nuovo= (lista) malloc(sizeof(lista));
    nuovo->student = i;
    nuovo->voto_ade = voto;
    nuovo->pref_gruppo = p;
    nuovo->nxt=NULL;
    return nuovo;
}

lista inserisci_in_testa(lista l, int i,int voto,int p){
    lista first = crea_nodo(i,voto,p);
    (*first).nxt = l;
    return first;
}

lista inserisci_in_coda(lista l,int i,int voto, int p){
    if(!(*l).nxt){
        lista new= crea_nodo(i,voto,p);
        (*l).nxt=new;
    }
    else {
        inserisci_in_coda((*l).nxt,i,voto,p);
    }
    return l;
}

lista rimuovi_in_testa (lista l){
    l=(*l).nxt;
    return l;
}

lista rimuovi_in_coda (lista l){
    if(!(*(*l).nxt).nxt){
        (*l).nxt=NULL;
    }
    else {
        rimuovi_in_coda((*l).nxt);
    }
    return l;
}

lista rimuovi_studente(lista l, int n){
    if(l->student != n && l->nxt != NULL){
        if(l->nxt->student == n){
            l-> nxt = l->nxt->nxt;
            return l;
        }
        else {
            rimuovi_studente(l->nxt, n);
        }
    }
    else if (l->student == n && l->nxt !=NULL){
        l=rimuovi_in_testa(l);
        return l;
    }
    else if (l->student == n && l->nxt == NULL){
        l=rimuovi_in_coda(l);
        return l;
    }
    return l;
}

int calc_pref(int pref_2,int pref_3,int pref_4){
    int x;
    time_t t;
    srand(getpid());
    x = rand()%100;
    printf("preferenza gruppo : %d\n",x);
    fflush(stdout);

    if(x >= pref_3 && x <= 100) {
        return 2;
    } else if(x < pref_3) {
        return 3;
    } else if(x < pref_4) {
        return 4;
    }
}
