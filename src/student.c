#include "common.h"

//variabili
int invites; //numero di inviti massimi che uno studente può inviare prima di ricevere una risposta
int nof_elem;
int leader;
int ade_mark; //voto di archittettura degli elaboratori
list requests; // list di iniviti o risposte ad inviti ricevute
list group;

int condition; // condizione uscita while principale

int mark_offset; //utility per massimizzare il voto
int group_num; //indica il numero di studenti in "group";
int final_mark; // esito finale

int reject; // numero di rifiuti effettuati
int id_student;

int main(int argc, char * argv[]) {
    signal(SIGCONT, child_handle_signal);

    init_ipc_id();
    init_student_parameters();

    request_resource(id_children_semaphore, 0);

    condition = 1;
    while(condition){
        if(group_num <= nof_elem){
            if(invites > 0 && requests == NULL && leader == 1){

                printf("(PID: %d) Sono lo studente %d\n",getpid(), id_student);
                printf("(PID: %d) Numero inviti : %d\n", getpid(), invites);
                printf("(PID: %d) Sono lo studente %d e sto inviando un invito \n", getpid(), id_student);
                
                // sending an invite
                costrutto.ask = 'W';
                costrutto.student_id = id_student;
                costrutto.mtype= id_student;
                costrutto.ade_voto = ade_mark;
                costrutto.pref_gruppo = nof_elem;
                msgsnd(id_message_queue,&costrutto,sizeof(costrutto),0);
                printf("(PID: %d) Invito inviato da studente %d \n",getpid(), id_student);
                invites--;

                sleep(1); // non cambiare 2 sec
            }

            msgrcv(id_message_queue,&costrutto,sizeof(costrutto),0,IPC_NOWAIT);

            if(costrutto.mtype != id_student && group_num == 1) {
                if(costrutto.ask =='W'){
                    if(requests == NULL) {
                        requests = create_node(costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                    }
                    else {
                        requests = insert_tail(requests,costrutto.student_id,costrutto.ade_voto,costrutto.pref_gruppo);
                    }
                }
            }

            sleep(1); // non cambiare 1 sec

            msgrcv(id_message_queue_answer,&costrutto3,sizeof(costrutto3),id_student,IPC_NOWAIT);

            if(leader==1){
                if(costrutto3.ask == 'S') {
                    if (!contains(group, costrutto3.student_id)) {
                        DEBUG;
                        group = insert_tail(group,costrutto3.student_id,costrutto3.ade_voto,costrutto3.pref_gruppo);
                        print_list(group);
                        invites++;
                        group_num++;
                        printf("(PID: %d) %d elementi sui %d desiderati\n",getpid(), group_num,nof_elem);
                    }
                }
                else if(costrutto3.ask == 'N') {
                    invites++;
                }
            }

            // while (requests!= NULL) {
            //   requests = remove_head(requests);
            // }

            read_invites(requests);
            requests = NULL;
        }
        else {
            condition = 0;
        }
    } // End while

    if(leader == 1) {
        printf(GRN "(PID: %d) [LEADER] Ho finito e aspetto!" RESET "\n", getpid());
    } else {
        printf(GRN "(PID: %d) [NORMAL] Ho finito e aspetto!" RESET "\n", getpid());
    }

    if(leader == 1){
        print_list(group);
        while (group != NULL) {
            costrutto2.mtype = getppid();
            costrutto2.sender = id_student;
            costrutto2.student_msg = group->student;
            costrutto2.ade_mark_msg = group->voto_ade;
            costrutto2.pref_gruppo_msg = group->pref_gruppo;
            costrutto2.group_id_msg = getpid() + 100;
            costrutto2.group_num_msg = group_num;

            msgsnd(id_message_queue_parent,&costrutto2,sizeof(costrutto2),0);

            printf(GRN"\t(PID: %d) Message sent to parent!"RESET"\n", getpid());
            group = group->nxt;
        }
    }

    request_resource(id_children_semaphore,0);

    int vero = 1;
    for(int i = 0;i<=POP_SIZE && vero; i++){
        if(shm_pointer->marks[i][0] == id_student){
            final_mark = shm_pointer->marks[i][4];
            vero = 0;
        }
    }

    printf("(PID: %d)" YEL "\tVoto AdE: %d" CYN "\tVoto SO: %d"RESET"\n", getpid(), ade_mark, final_mark);
    exit(EXIT_SUCCESS);
}

void read_invites(list inviti){

    request_resource(id_rw_semaphore, 0);

    while(inviti!=NULL){
        if(group_num == 1){
            if((*inviti).student%2 == id_student%2){
                printf(GRN "(PID: %d) Invito ricevuto da parte dello studente %d" RESET "\n", getpid(), (*inviti).student);
                printf("(PID: %d) Group_num = %d\n", getpid(), group_num);
                printf("(PID: %d) Voto = %d\n",getpid(), (*inviti).voto_ade);
                if((*inviti).voto_ade >= (30 - mark_offset)){
                    costrutto3.ask = 'S';
                    leader = 0;
                    group_num++;
                    costrutto3.mtype=(*inviti).student;
                    costrutto3.student_id = id_student;
                    costrutto3.ade_voto = ade_mark;
                    costrutto3.pref_gruppo = nof_elem;

                    msgsnd(id_message_queue_answer,&costrutto3,sizeof(costrutto3),0);
                    printf(GRN "(PID: %d) Risposta affermativa da parte dello studente %d allo studente %d" RESET"\n",getpid(), id_student,(*inviti).student);
                    printf(CYN "(PID: %d) Il leader dello studente %d ora è %d" RESET"\n",getpid(), id_student,(*inviti).student);
                }   
                else{
                    if(reject>0){
                        costrutto3.mtype=(*inviti).student;
                        costrutto3.student_id = id_student;
                        costrutto3.ask = 'N';
                        costrutto3.ade_voto = ade_mark;
                        costrutto3.pref_gruppo = nof_elem;
                        msgsnd(id_message_queue_answer,&costrutto3,sizeof(costrutto3),0);
                        printf(RED "(PID: %d) Risposta negativa da parte dello studente %d allo studente %d" RESET "\n",getpid(), id_student,(*inviti).student);
                        reject--;
                        printf("(PID: %d) Rifiuti rimasti allo studente %d : %d \n",getpid(), id_student,reject);

                        mark_offset= mark_offset + 2;
                    }
                    else{
                        leader = 0;
                        costrutto3.ask = 'S';
                        group_num++;
                        costrutto3.mtype=(*inviti).student;
                        costrutto3.student_id = id_student;
                        costrutto3.ade_voto = ade_mark;
                        costrutto3.pref_gruppo = nof_elem;
                        msgsnd(id_message_queue_answer,&costrutto3,sizeof(costrutto3),0);
                        printf(YEL "(PID: %d) Risposta affermativa da parte dello studente %d allo studente %d" RESET "\n",getpid(), id_student,(*inviti).student);
                    }
                }
            }
        }
        else{
            costrutto3.mtype=(*inviti).student;
            costrutto3.student_id = id_student;
            costrutto3.ask = 'N';
            costrutto3.ade_voto = ade_mark;
            costrutto3.pref_gruppo = nof_elem;
            msgsnd(id_message_queue_answer,&costrutto3,sizeof(costrutto3),0);
            printf("(PID: %d) Risposta negativa da parte dello studente %d allo studente %d \n",getpid(), id_student,(*inviti).student);
        }
        inviti = inviti->nxt;
    } // end while

    relase_resource(id_rw_semaphore, 0);
}

void child_handle_signal(int signal) {
    condition = 0;
}

void init_student_parameters(){
    read_conf(CONF_PATH);
    id_student = getpid();
    leader = 1;
    requests = NULL;
    invites = nof_invites;
    ade_mark = random_between(getpid(), 18, 30);
    nof_elem = compute_preference(dev_preference_2,dev_preference_3,dev_preference_4 );
    final_mark = 0;
    group = create_node(id_student, ade_mark, nof_elem);
    group_num = 1; // at the beginning it counts himself
    mark_offset=0;
    reject = max_reject;
}

void init_ipc_id(){
    // semaphore
    id_children_semaphore = semget(KEY_CHILDREN_SEMAPHORE, 1, IPC_CREAT | 0666);
    id_rw_semaphore = semget(KEY_RW_SEMAPHORE, 1, IPC_CREAT | 0666);

    // message queue
    init_message_queue(KEY_MESSAGE_QUEUE);
    id_message_queue_parent = msgget(KEY_MESSAGE_QUEUE_PARENT,IPC_CREAT | 0666);
    id_message_queue_answer = msgget(KEY_MESSAGE_QUEUE_ANSWER,IPC_CREAT | 0666);

    // shared memory
    id_shared_memory = shmget(KEY_SHARED_MEMORY, sizeof(*shm_pointer), 0666 | IPC_CREAT);

    // shared memory pointer
    shm_pointer = /*(struct shared_data *)*/ shmat(id_shared_memory, NULL, 0);
    if (shm_pointer == (void *) -1) {
        PRINT_ERROR;
    }
}
