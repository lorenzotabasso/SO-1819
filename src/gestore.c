//Gestore

#define POP_SIZE 4

int main() {
	pid_t my_pid, my_ppid, value;
	int var = 4

	switch (value = fork()) {
		case -1:
			/* Handle error */
			fprintf(stderr,"Error #%03d: %s\n", errno, strerror(errno));
			break;
			
		case 0:
			/* Perform actions specific to child */
			my_pid = getpid();
			my_ppid = getppid();
			printf("CHILD:  PID=%d, PPID=%d, fork_value=%d\n",
			       my_pid, my_ppid, value);
			//sleep(2);   /* "waiting" the parent proc to modify var */
			printf("CHILD: var=%d\n", var);
			break;
			
		default:
			/* Perform actions specific to parent */
			my_pid = getpid();
			my_ppid = getppid();
			printf("PARENT: PID=%d, PPID=%d, fork_value=%d\n",
			       my_pid, my_ppid, value);
			var = 8;
			printf("PARENT: var=%d\n", var);
			wait();
			printf("ATTENDING...");
			break;
		}
		/* Both child and parent process will execute here!! */ 
		exit(EXIT_SUCCESS);
}
