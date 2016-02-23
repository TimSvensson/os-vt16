/**
 * Game of luck: Implementation of the Gamemaster
 *
 * Course: Operating Systems and Multicore Programming - OSM lab
 * assignment 1: game of luck.
 *
 * Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
 *
 * History
 *
 * 2016-01-31 (Mahdad Davari)
 *
 * Added more annotations and fine-grained TODO lists 
 *
 */

#include <stdio.h> /* I/O functions: printf() ... */
#include <stdlib.h> /* rand(), srand() */
#include <unistd.h> /* read(), write() calls */
#include <assert.h> /* assert() */
#include <time.h>   /* time() */
#include <signal.h> /* kill(), raise() and SIG???? */

#include <sys/types.h> /* pid */
#include <sys/wait.h> /* waitpid() */

#include "common.h"

#define READ  0
#define WRITE 1

#define NEW_STDIN(A)  dup2(stdin, pa.p[A].write[READ])
#define NEW_STDOUT(A) dup2(stdout, pa.p[A].read[WRITE])

#define P_WRITE(A) pa.p[A].write[WRITE]
#define P_READ(A)  pa.p[A].read[READ]

typedef struct _pipeArray pipeArray;
typedef struct _pipes pipes;

struct _pipes {
	int read[2];
	int write[2];
};

struct _pipeArray {
	pipes p[NUM_PLAYERS];
};

int main(int argc, char *argv[])
{
	int i, seed;

	// TODO 1: Un-comment the following variables to use them in the 
	//         exec system call. Using the function sprintf and the arg1 
	//         variable you can pass the id parameter to the children 

	char arg0[] = "./shooter";
	char arg1[10]; 
	char *args[] = {arg0, arg1, NULL};
	
	// TODO 2: Declare pipe variables
	//         - Of which data type should they be?
	//         - How many pipes are needed?
	//         - Try to choose self-explanatory variable names, e.g. seedPipe, scorePipe

	pipeArray pa;
	int id_pid[NUM_PLAYERS] = {0};

	// TODO 3: initialize the communication with the players, i.e. create the pipes
	for (i = 0; i < NUM_PLAYERS; i++) {
		pipe(pa.p[i].read);
		pipe(pa.p[i].write);
	}

	// TODO 4: spawn/fork the processes that simulate the players
	//         - check if players were successfully spawned
	//         - is it needed to store the pid of the players? Which data structure to use for this?
	//         - re-direct standard-inputs/-outputs of the players
	//         - use execv to start the players
	//         - pass arguments using args and sprintf

	printf("master: Forking...\n");
	for (i = 0; i < NUM_PLAYERS; i++) {
		pid_t pid;
		switch(pid = fork()) {
		case -1:
			// error
			perror("FORK");
			exit(EXIT_FAILURE);
			break;
		case 0:
			// child
			// printf("\tC<%d> Closing pipes\n", getpid());
			for (int j = 0; j < NUM_PLAYERS; j++) {
				if (i == j) {
					// printf("\tC<%d> Overwriting stdin and stdout.\n", getpid());
					if(dup2(pa.p[j].write[READ], 0) < 0) {
						perror("stdin-DUP2");
						exit(EXIT_FAILURE);
					}
					if(dup2(pa.p[j].read[WRITE], 1) < 0) {
						perror("stdout-DUP2");
						exit(EXIT_FAILURE);
					}					
				}
				close(pa.p[j].read[READ]);
				close(pa.p[j].read[WRITE]);

				close(pa.p[j].write[READ]);
				close(pa.p[j].write[WRITE]);
			}
			sprintf(arg1, "%d", i);

			execv(arg0, args);
			break;
		default:
			// parent
			// printf("P<%d> <%d> is my child.\n", getpid(), pid);
			
			id_pid[i] = pid;

			close(pa.p[i].read[WRITE]);
			close(pa.p[i].write[READ]);
		}
	}

	// sleep(1);
	// printf("\nP<%d> All children forked.\n\n", getpid());
	
	printf("master: Seeding...\n");
	seed = time(NULL);

	for (i = 0; i < NUM_PLAYERS; i++) {
		seed++;

		// char str[50] = {'\0'};
		// read(pa.p[i].read[READ], str, 50);
		// printf("P<%d> A child said:\n%s", getpid(), str);

		// TODO 5: send the seed to the players (write using pipes)		
		write(pa.p[i].write[WRITE], &seed, sizeof(int));
	}


	// TODO 6: read the dice results from the players via pipes, find the winner


	printf("master: Getting results...\n");
	int result[NUM_PLAYERS] = {0};
	for (i = 0; i < NUM_PLAYERS; i++) {
		read(pa.p[i].read[READ], &result[i], sizeof(int));
	}

	int winner = 0;
	for (int i = 0; i < NUM_PLAYERS; i++) {
		if (result[winner] < result[i]) {
			winner = i;
		}
	}

	printf("master: player %d WINS\n", winner);

	// TODO 7: signal the winner
	//         - which command do you use to send signals?
	//         - you will need the pid of the winner
	
	kill(id_pid[winner], SIGUSR1);

	// TODO 8: signal all players the end of game
	//         - you will need the pid of all the players


	printf("master: the game ends\n");
	for (i = 0; i < NUM_PLAYERS; i++) {
		kill(id_pid[i], SIGUSR2);
	}

	// TODO 9: cleanup resources and exit with success
	//         wait for all the players/children to exit
	//         before game master exits 

	for (i = 0; i < NUM_PLAYERS; i++) {
		wait(NULL);
	}

	return 0;
}
