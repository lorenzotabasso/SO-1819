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

struct sigaction sa;

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
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;

    set_rand_ade_mark();
    printf("CHILD (PID: %d): Hi! I'm working! ADE_Mark: %d\n", getpid(), student.ade_mark);

    // Set this handler for all signals
	for (int i=0; i<NSIG; i++) {
		if (sigaction(i, &sa, NULL) == -1) {
			fprintf(stderr, "Cannot set a user-defined handler for Signal #%d: %s\n", i, strsignal(i));
		}
	}
    
    while(1){

    }

    //signal(SIGCONT, goodbye); // DECOMMENT ONLY AT THE END
    //exit(EXIT_SUCCESS);
}

void handle_signal(int signal) {
	printf("Got signal #%d: %s\n", signal, strsignal(signal));
    printf("CHILD (PID: %d): goodbye cruel word! :(", getpid());
    exit(EXIT_SUCCESS);
}