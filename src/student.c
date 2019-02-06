#include "common.h"

//variabili
int invites; //numero di inviti massimi che uno studente può inviare prima di ricevere una risposta
int nof_elem;
int leader;
int ade_mark; //voto di archittettura degli elaboratori
list requests; // list di iniviti o risposte ad inviti ricevute
list group;

int condition; // condizione uscita while principale

int scarto_voto; //utility per massimizzare il voto
int group_num; //indica il numero di studenti in "group";
int final_mark; // esito finale

int reject; // numero di rifiuti effettuati
int id_student;

int main(int argc, char * argv[]) {
    signal(SIGCONT, handle_signal);

    init_ipc_id();

    init_student_parameters();
    condition = 1;

    request_resource(id_children_semaphore, 0);

    while(condition){
        if(invites > 0 && requests == NULL && leader == 1){
            if(group_num <= nof_elem){
                printf("(PID: %d) Sono lo studente %d\n",getpid(), id_student);
                printf("(PID: %d) Numero inviti : %d\n", getpid(), invites);
                printf("(PID: %d) %d elementi sui %d desiderati\n",getpid(), group_num,nof_elem);

                printf("(PID: %d) Sono lo studente %d e sto inviando un invito \n", getpid(), id_student);
                if (random_between(getpid(), 0, 1)) {
                    invia_invito();
                } else {
                    sleep(1);
                }
                //invites--; // TODO: Errore! spostare in invia_invito()
            }
        }

        msgrcv(id_message_queue,&costrutto,sizeof(costrutto),0,0);

        if(costrutto.mtype != id_student && group_num == 1) {
            if(costrutto.ask =='W'){
                if(requests == NULL) {
                    requests = crea_nodo(costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                }
                else {
                    requests = inserisci_in_coda(requests,costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                }
            }
        }
        else if(costrutto.mtype == id_student){
            if(costrutto.ask == 'S') {
                    group = inserisci_in_coda(group,costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                    invites++;
                    group_num++;

            }
            else if(costrutto.ask == 'N') {
                invites++;
            }
        }
        requests = leggi_inviti(requests);
    } // End while

    if(leader == 1)
        printf(GRN "(PID: %d)  LEADER Ho finito e aspetto!" RESET "\n", getpid());
    else
        printf(GRN "(PID: %d)  NORMAL Ho finito e aspetto!" RESET "\n", getpid());

    if(leader == 1){
        costrutto2.mtype = getppid();
        costrutto2.sender = id_student;
        printf("sender : %d\n",costrutto2.sender);
        costrutto2.gruppo = group;
        stampa_list(costrutto2.gruppo);
        costrutto2.group_nums = group_num;
        printf("group num: %d\n",costrutto2.group_nums);
        msgsnd(id_message_queue_parent,&costrutto2,sizeof(costrutto2),0);
    }

    printf(YEL "(PID: %d) SEMAPHORE RES: %d" RESET "\n", getpid(), semctl(id_children_semaphore, 0, GETVAL));

    request_resource(id_children_semaphore,0);

    // // shared memory
    // id_shared_memory = shmget(KEY_SHARED_MEMORY, sizeof(*shm_pointer), 0666 | IPC_CREAT);
    // // shared memory pointer
    // shm_pointer = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    // if (shm_pointer == (void *) -1) {
    //     PRINT_ERROR;
    // }

    int vero = 1;
    for(int i = 0;i<=POP_SIZE && vero; i++){
        if(marks[i][0] == id_student){
            final_mark = marks[i][1];
            vero = 0;
        }
    }

    printf(YEL "(PID: %d) Sono lo studente : %d" RESET "\n", getpid(), id_student);
    printf("(PID: %d) avevo preso %d di archittettura degli elaboratori\n", getpid(), ade_mark);
    printf("(PID: %d) e ho preso %d di sistemi operativi\n", getpid(), final_mark);
}

void invia_invito(){
    costrutto.ask = 'W';
    costrutto.student_id = id_student;
    costrutto.mtype= id_student;
    costrutto.ade_voto = ade_mark;
    costrutto.pref_gruppo = nof_elem;
    msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
    printf("(PID: %d) Invito inviato da studente %d \n",getpid(), id_student);
    invites--;
}

list leggi_inviti(list inviti){
    while(inviti!=NULL){
        if(group_num == 1){
            if((*inviti).student%2 == id_student%2 ){
                printf(GRN "(PID: %d) Invito ricevuto da parte dello studente %d" RESET "\n", getpid(), (*inviti).student);
                printf("(PID: %d) Group_num = %d\n", getpid(), group_num);
                printf("(PID: %d) Voto = %d\n",getpid(), (*inviti).voto_ade);
                if((*inviti).voto_ade >= (30 - scarto_voto)){
                    costrutto.ask = 'S';
                    leader = 0;
                    group_num++;
                    costrutto.mtype=(*inviti).student;
                    costrutto.student_id = id_student;
                    costrutto.ade_voto = ade_mark;
                    costrutto.pref_gruppo = nof_elem;

                    msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
                    printf(GRN "(PID: %d) Risposta affermativa da parte dello studente %d allo studente %d" RESET"\n",getpid(), id_student,(*inviti).student);
                }
                else{
                    if(reject>0){
                        costrutto.mtype=(*inviti).student;
                        costrutto.student_id = id_student;
                        costrutto.ask = 'N';
                        costrutto.ade_voto = ade_mark;
                        costrutto.pref_gruppo = nof_elem;
                        msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
                        printf(RED "(PID: %d) Risposta negativa da parte dello studente %d allo studente %d" RESET "\n",getpid(), id_student,(*inviti).student);
                        reject--;
                        printf("(PID: %d) Rifiuti rimasti allo studente %d : %d \n",getpid(), id_student,reject);

                        scarto_voto= scarto_voto + 2;
                    }
                    else{
                        leader = 0;
                        costrutto.ask = 'S';
                        group_num++;
                        costrutto.mtype=(*inviti).student;
                        costrutto.student_id = id_student;
                        costrutto.ade_voto = ade_mark;
                        costrutto.pref_gruppo = nof_elem;
                        msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
                        printf(YEL "(PID: %d) Risposta affermativa da parte dello studente %d allo studente %d" RESET "\n",getpid(), id_student,(*inviti).student);
                    }
                }
            }
        }
        else{
            costrutto.mtype=(*inviti).student;
            costrutto.student_id = id_student;
            costrutto.ask = 'N';
            costrutto.ade_voto = ade_mark;
            costrutto.pref_gruppo = nof_elem;
            msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
            printf("(PID: %d) Risposta negativa da parte dello studente %d allo studente %d \n",getpid(), id_student,(*inviti).student);
            //printf("Rifiuti rimasti allo studente %d : %d \n",id_student,reject);
        }

        inviti = rimuovi_in_testa(inviti);
    } // end while
    return inviti;
}

void handle_signal(int signal) {
    condition = 0;
}

void init_student_parameters(){
    read_conf(CONF_PATH);

    id_student = getpid();
    leader = 1;
    printf("(PID: %d) ID studente: %d\n",getpid(), id_student);
    requests = NULL;
    invites = nof_invites;
    ade_mark = random_between(getpid(), 18, 30);
    nof_elem = calc_pref(dev_preference_2,dev_preference_3,dev_preference_4 );
    final_mark = 0;
    group = crea_nodo(id_student, ade_mark, nof_elem);
    group_num = 1;
    scarto_voto=0;
    reject = max_reject;
}

void init_ipc_id(){
    // semaphore
    id_children_semaphore = semget(KEY_CHILDREN_SEMAPHORE, 1, IPC_CREAT | 0666);

    // message queue
    init_message_queue(KEY_MESSAGE_QUEUE);
    id_message_queue_parent = msgget(KEY_MESSAGE_QUEUE_PARENT,IPC_CREAT | 0666);
}

void goodbye(){
    printf("(PID: %d): goodbye cruel word!\n", getpid());
    exit(EXIT_SUCCESS);
}
