#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void fork3() {
	int status;
	if (fork() == 0) {
		printf("HC\n");
	}
	else {
		printf("HP\n");
		wait(&status);
		printf("%d\n", status);
		printf("CT\n");
	}
	printf("Bye\n");
	exit(-1);
}

int main()
{
	fork3();
	return 0;
}