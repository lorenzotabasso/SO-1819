#include "common.h"

struct stud {
    int id;
    int ade_mark;
    int nof_elem;
    int ask;
    int nof_invites;
    int max_reject;
    int final_mark;
} student;

void set_rand_ade_mark(){
    student.ade_mark = random_between(getpid(), 18, 30);
}

void find_mates(){

}

// it terminates this process;
void goodbye(){
    printf("CHILD (PID: %d): goodbye cruel word! :(", getpid());
    exit(EXIT_SUCCESS);
}

int main(int argc, char * argv[]) {
    set_rand_ade_mark();
    printf("CHILD (PID: %d): Hi! I'm working! ADE_Mark: %d\n", getpid(), student.ade_mark);

    // pause();

    // while(1){
        
    // }
    
    exit(EXIT_SUCCESS);

    //signal(SIGCONT, goodbye); // DECOMMENT ONLY AT THE END
}
