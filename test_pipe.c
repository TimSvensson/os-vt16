#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1

void child(int fildes[2])
{
	printf("    C<%d> I am the child of <%d>.\n", getpid(), getppid());
	
	printf("    C<%d> I will now read my end of the pipe!\n", getpid());

	int buf = 0;
	int ret = -1;
	read(fildes[READ], &buf, sizeof(buf));
	
	printf("    C<%d> My parent told me: %d.\n", getpid(), buf);
	printf("    C<%d> Goodbye!\n", getpid());
}

void parent(pid_t child_pid, int fildes[2])
{
	printf("P<%d> I am the parent!\n", getpid());

	printf("P<%d> I will now write '7' to the pipe.\n", getpid());
	int i = 7;
	ssize_t w_ret = -1;
	w_ret = write(fildes[WRITE], &i, sizeof(int));
	printf("P<%d> w_ret = %d.\n", getpid(), (int) w_ret);

	printf("P<%d> I will now wait for <%d>.\n", getpid(), child_pid);
	waitpid(child_pid, NULL, 0);
	printf("P<%d> I have waited for my child to exit.\n", getpid());

	printf("P<%d> Goodbye!\n", getpid());
}

int main(int argc, char **argv)
{
	printf("\n");

	pid_t pid = fork();

	int fildes[2];
	pipe(fildes);

	if (pid == -1) {
		// error
		return(-1);
	} else if (pid == 0) {
		// child

		child(fildes);
	} else {
		// parent

		parent(pid, fildes);
	}

	printf("\n");

	return(0);
}
