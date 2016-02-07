#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define READ 0
#define WRITE 1

void sig_handler(int sig)
{
	signal(SIGINT, sig_handler);
	printf("SIGPIPE\n");
	fflush(stdout);
}

void child(int fildes[2])
{
	if (close(fildes[WRITE]) < 0) {
		perror("Unable to close WRITE-pipe!");
		exit(-1);
	}

	printf("    C<%d> I am the child of <%d>.\n", getpid(), getppid());	
	printf("    C<%d> I will now read my end of the pipe!\n", getpid());

	int buf = 0;
	if (read(fildes[READ], &buf, sizeof(buf)) < 0) {
		perror("READ FAILED");
		exit(-1);
	}
	
	printf("    C<%d> My parent told me: %d.\n", getpid(), buf);
	printf("    C<%d> Goodbye!\n", getpid());
}

void parent(pid_t child_pid, int fildes[2])
{
	if (close(fildes[READ]) < 0) {
		perror("Unable to close READ-pipe!");
		exit(-1);
	}

	printf("P<%d> I am the parent!\n", getpid());
	printf("P<%d> I will now write '7' to the pipe.\n", getpid());

	int buf = 7;
	if (write(fildes[WRITE], &buf, sizeof(buf)) < 0) {
		perror("FAILED TO WRITE!");
		exit(-1);
	}

	printf("P<%d> I will now wait for <%d>.\n", getpid(), child_pid);
	waitpid(child_pid, NULL, 0);
	printf("P<%d> I have waited for my child to exit.\n", getpid());

	printf("P<%d> Goodbye!\n", getpid());
}

int main(int argc, char **argv)
{
	printf("\n");

	signal(sig_handler, SIGPIPE);

	int fildes[2];
	pipe(fildes);
	pid_t pid = fork();

	pid_t pid = fork();

	if (pid == -1) {
		// error
		return(-1);
	} else if (pid == 0) {
		// child

		child(fildes);
		exit(0);
	} else {
		// parent

		parent(pid, fildes);
	}

	printf("\n");

	return(0);
}
