#include "common.h"

//variabili
int msg; //identificativo coda di messaggi
int semaforo; //identificativo semaforo
int invites; //numero di inviti massimi che uno studente può inviare prima di ricevere una risposta
int nof_elem;
int leader;
int ade_mark; //voto di archittettura degli elaboratori
lista requests; // lista di iniviti o risposte ad inviti ricevute
lista group;

int condition; // condizione uscita while principale

int scarto_voto; //utility per massimizzare il voto
int group_num; //indica il numero di studenti in "group";
int final_mark; // esito finale

int reject; // numero di rifiuti effettuati
int id_student;
int msg_answer;
//*************//

//funzioni studente
void initialize_info(); // inizializza le variabili  dello studente
void goodbye(int i);
void invia_invito(); // invia inviti per unirsi al suo gruppo
lista leggi_inviti(lista l); // legge la lista contenente gli inviti ricevuti
lista leggi_risposte(lista k); // contiene le risposte agli inviti dello studente
//***********/

void set_rand_ade_mark(){
    student.ade_mark = random_between(getpid(), 18, 30);
}

int main(int argc, char * argv[]) {
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;

    // setting the IDs of children_semaphore and shared_memory in the child
    // (if we don't do that here, both semaphore and shared memory IDs will 
    // be = 0 and all the operation on sempahores will exit with an error).
    id_children_semaphore = semget(KEY_CHILDREN_SEM, 1, IPC_CREAT | 0666);
    id_shared_memory = shmget(KEY_SHARED_MEM, sizeof(*my_shm), 0666 | IPC_CREAT);

    my_shm = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (my_shm == (void *) -1) {
        PRINT_ERROR;
    }

    init_student();
    condition = 1;

    /* Child will remain blocked until the father will fill the semaphore 
     * with enougth resources to unlock them all.
     */
    request_resource(id_children_semaphore, 0);

    
    while(condition){
        if(invites > 0 && requests == NULL && leader == 1){
            if(group_num <= nof_elem){
                printf("Sono lo studente %d\n",id_student);
                printf("Numero inviti : %d\n",invites);
                printf("%d elementi sui %d desiderati\n",group_num,nof_elem);

                printf("Sono lo studente %d e sto inviando un invito \n", id_student);
                fflush(stdout);
                if (random_between(getpid(), 0, 1)) {
                    invia_invito();
                } else {
                    sleep(2);
                }
                invites--;
            }
        }
        else {
            verita = 0;
        }

        msgrcv(msg,&costrutto,sizeof(costrutto),0,0666);

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

    printf(GRN "Ho finito e aspetto!" RESET "\n");

    request_resource(semaforo,0);

    if(leader == 1){
        costrutto2.mtype = getppid();
        costrutto2.gruppo = group;
        msgsnd(msg,&costrutto2,sizeof(costrutto2),0);
    }
    int vero = 1;
    for(int i = 0;i<=POP_SIZE && vero; i++){
        if(my_shm->marks[i][1] == id_student){
            final_mark = my_shm->marks[i][2];
            vero = 0;
        }
    }

    printf(YEL "Sono lo studente : %d" RESET "\n",id_student);
    printf("avevo preso %d di archittettura degli elaboratori\n",ade_mark);
    printf("e ho preso %d di sistemi operativi\n",final_mark);

}

void invia_invito(){
    sleep(3);
    costrutto.ask = 'W';
    costrutto.student_id = id_student;
    costrutto.mtype= id_student;
    costrutto.ade_voto = ade_mark;
    costrutto.pref_gruppo = nof_elem;
    msgsnd(msg,&costrutto,sizeof(costrutto),0);
    printf("Invito inviato da studente %d \n",id_student);
    fflush(stdout);
}

lista leggi_inviti(lista inviti){
    while(inviti!=NULL && group_num == 1){
        if(group_num == 1){
            fflush(stdout);
            sleep(1);
            if((*inviti).student%2 == id_student%2 ){
                printf("Invito ricevuto da parte dello studente %d\n",(*inviti).student);
                printf("Group_num = %d\n",group_num);
                printf("Voto = %d\n",(*inviti).voto_ade);
                fflush(stdout);
                if((*inviti).voto_ade >= (30 - scarto_voto)){
                    costrutto.ask = 'S';
                    leader = 0;
                    group_num++;
                    costrutto.mtype=(*inviti).student;
                    costrutto.student_id = id_student;
                    costrutto.ade_voto = ade_mark;
                    costrutto.pref_gruppo = nof_elem;

                    msgsnd(msg,&costrutto,sizeof(costrutto),0);
                    printf(GRN "Risposta affermativa da parte dello studente %d allo studente %d" RESET"\n",id_student,(*inviti).student);
                    fflush(stdout);
                }
                else{
                    if(reject>0){
                        costrutto.mtype=(*inviti).student;
                        costrutto.student_id = id_student;
                        costrutto.ask = 'N';
                        costrutto.ade_voto = ade_mark;
                        costrutto.pref_gruppo = nof_elem;
                        msgsnd(msg,&costrutto,sizeof(costrutto),0);
                        printf("Risposta negativa da parte dello studente %d allo studente %d \n",id_student,(*inviti).student);
                        reject--;
                        printf("Rifiuti rimasti allo studente %d : %d \n",id_student,reject);
                        fflush(stdout);

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
                        msgsnd(msg,&costrutto,sizeof(costrutto),0);
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
            msgsnd(msg,&costrutto,sizeof(costrutto),0);
            printf("Risposta negativa da parte dello studente %d allo studente %d \n",id_student,(*inviti).student);
            //printf("Rifiuti rimasti allo studente %d : %d \n",id_student,reject);
        }

        inviti = rimuovi_in_testa(inviti);
        fflush(stdout);
    } // end while
    return inviti;
}

void handle_signal(int signal) {
    printf("CHILD (PID: %d): got signal #%d: %s\n", getpid(), signal, strsignal(signal));
    switch (signal) {
        case SIGALRM:
            verita = 0;
            break;
        default:
            break;
    }
}

void init_student(){
    read_conf("opt.conf");

    id_student = getpid();
    leader = 1;
    printf("ID studente: %d\n",id_student);
    //fflush(stdout);
    requests = NULL;
    invites = nof_invites;
    ade_mark = random_between(getpid(), 18, 30);
    nof_elem = calc_pref(dev_preference_2,dev_preference_3,dev_preference_4 );
    final_mark = 0;
    group = NULL;
    group_num = 1;
    scarto_voto=0;
    reject = max_reject;
    //id_message_queue = msgget(key_msg,IPC_CREAT | 0666);
    init_message_queue(KEY_MESSAGE_QUEUE);
    // se usassimo il PID come chiave?
    //id_children_semaphore = semget(key_children_semaphore,1,IPC_CREAT|0666);
    init_children_semaphore(KEY_CHILDREN_SEM);
}

void goodbye(int i){
    printf("CHILD (PID: %d): goodbye cruel word!\n", getpid());
    printf("\tContinuing - 2\n");
    exit(EXIT_SUCCESS);
}
