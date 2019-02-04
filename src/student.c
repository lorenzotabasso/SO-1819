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
//*************//

 // TODO: problemi di concorrenza tra i tipi di messaggi!

int main(int argc, char * argv[]) {
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;

    init_ipc_id();

    init_student_parameters();
    condition = 1;

    request_resource(id_children_semaphore, 0);
    DEBUG;

    while(condition){
        if(invites > 0 && requests == NULL && leader == 1){
            if(group_num <= nof_elem){
                printf("Sono lo studente %d\n",id_student);
                printf("Numero inviti : %d\n",invites);
                printf("%d elementi sui %d desiderati\n",group_num,nof_elem);

                printf("Sono lo studente %d e sto inviando un invito \n", id_student);
                if (random_between(getpid(), 0, 1)) {
                    invia_invito();
                } else {
                    sleep(1);
                }
                invites--;
            }
        }
        else {
            condition = 0;
        }

        msgrcv(id_message_queue,&costrutto,sizeof(costrutto),0,0666);

        if(group_num == 1 && costrutto.mtype != id_student) {
            if(costrutto.ask =='W'){
                if(requests == NULL) {
                    requests = crea_nodo(costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                }
                else {
                    requests = inserisci_in_coda(requests,costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                }
            }
            DEBUG;
        }
        else if(costrutto.mtype == id_student){
            DEBUG;
            if(costrutto.ask == 'S') {
                DEBUG;
                if(group == NULL){
                    group = crea_nodo(costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                    invites++;
                }
                else{
                    group = inserisci_in_coda(group,costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                    invites++;
                }
            }
            else if(costrutto.ask == 'N') {
                DEBUG;
                invites++;
            }
            DEBUG;
        }
        requests = leggi_inviti(requests);
    }

    printf(GRN "(%d) Ho finito e aspetto!" RESET "\n", getpid());



    if(leader == 1){
        costrutto2.mtype = getppid();
        costrutto2.gruppo = group;
        costrutto2.group_num = group_num;
        msgsnd(id_message_queue,&costrutto2,sizeof(costrutto2),0);
    }

    printf(YEL "SEMAPHORE RES: %d" RESET "\n", semctl(id_children_semaphore, 0, GETVAL));

    request_resource(id_children_semaphore,0);

    int vero = 1;
    for(int i = 0;i<=POP_SIZE && vero; i++){
        if(shm_pointer->marks[i][1] == id_student){
            final_mark = shm_pointer->marks[i][1];
            vero = 0;
        }
    }

    printf(YEL "Sono lo studente : %d" RESET "\n",id_student);
    printf("avevo preso %d di archittettura degli elaboratori\n",ade_mark);
    printf("e ho preso %d di sistemi operativi\n",final_mark);

}

void invia_invito(){
    costrutto.ask = 'W';
    costrutto.student_id = id_student;
    costrutto.mtype= id_student;
    costrutto.ade_voto = ade_mark;
    costrutto.pref_gruppo = nof_elem;
    msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
    printf("Invito inviato da studente %d \n",id_student);
}

list leggi_inviti(list inviti){
    while(inviti!=NULL && group_num == 1){
        if(group_num == 1){
            if((*inviti).student%2 == id_student%2 ){
                printf("Invito ricevuto da parte dello studente %d\n",(*inviti).student);
                printf("Group_num = %d\n",group_num);
                printf("Voto = %d\n",(*inviti).voto_ade);
                if((*inviti).voto_ade >= (30 - scarto_voto)){
                    costrutto.ask = 'S';
                    leader = 0;
                    group_num++;
                    costrutto.mtype=(*inviti).student;
                    costrutto.student_id = id_student;
                    costrutto.ade_voto = ade_mark;
                    costrutto.pref_gruppo = nof_elem;

                    msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
                    printf(GRN "Risposta affermativa da parte dello studente %d allo studente %d" RESET"\n",id_student,(*inviti).student);
                }
                else{
                    if(reject>0){
                        costrutto.mtype=(*inviti).student;
                        costrutto.student_id = id_student;
                        costrutto.ask = 'N';
                        costrutto.ade_voto = ade_mark;
                        costrutto.pref_gruppo = nof_elem;
                        msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
                        printf("Risposta negativa da parte dello studente %d allo studente %d \n",id_student,(*inviti).student);
                        reject--;
                        printf("Rifiuti rimasti allo studente %d : %d \n",id_student,reject);

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
                        printf("Risposta affermativa da parte dello studente %d allo studente %d \n",id_student,(*inviti).student);
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
            printf("Risposta negativa da parte dello studente %d allo studente %d \n",id_student,(*inviti).student);
            //printf("Rifiuti rimasti allo studente %d : %d \n",id_student,reject);
        }

        inviti = rimuovi_in_testa(inviti);
    } // end while
    return inviti;
}

void handle_signal(int signal) {
    DEBUG;
    printf("CHILD (PID: %d): got signal #%d: %s\n", getpid(), signal, strsignal(signal));
    switch (signal) {
        case SIGALRM:
            condition = 0;
            DEBUG;
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}

void init_student_parameters(){
    read_conf("src/opt.conf");

    id_student = getpid();
    leader = 1;
    printf("ID studente: %d\n",id_student);
    requests = NULL;
    invites = nof_invites;
    ade_mark = random_between(getpid(), 18, 30);
    nof_elem = calc_pref(dev_preference_2,dev_preference_3,dev_preference_4 );
    final_mark = 0;
    group = NULL;
    group_num = 1;
    scarto_voto=0;
    reject = max_reject;
}

void init_ipc_id(){
    // semaphore
    id_children_semaphore = semget(KEY_CHILDREN_SEMAPHORE, 1, IPC_CREAT | 0666);

    // message queue
    init_message_queue(KEY_MESSAGE_QUEUE);

    // shared memory
    id_shared_memory = shmget(KEY_SHARED_MEMORY, sizeof(*shm_pointer), 0666 | IPC_CREAT);

    // shared memory pointer
    shm_pointer = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (shm_pointer == (void *) -1) {
        PRINT_ERROR;
    }
}

void goodbye(){
    printf("CHILD (PID: %d): goodbye cruel word!\n", getpid());
    exit(EXIT_SUCCESS);
}
