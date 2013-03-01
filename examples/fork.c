#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
    pid_t childpid, mypid;
    int i, j;
    int status;
    int runparent = 10, runchild = 6;

    /**
     * Upon successful completion, fork() returns a value of 0 to the child
     * process and returns the process ID of the child process to the parent
     * process.  Otherwise, a value of -1 is returned to the parent process, no
     * child process is created, and the global variable errno is set to indi-
     * cate the error. */

    if (argc > 1) {
	    runparent = 6;
	    runchild = 10;
    }

    i = j = 23;

    childpid = fork();

    switch (childpid) {
    case -1:
	err(1, "fork() failed");
	/* NOTREACHED */

    default:
	mypid = getpid();
	for (i = 0; i < runparent; i++) {
	    printf("Hello from the parent. I have pid %d and my child has %d. i=%d, j=%d\n",
		   (int)mypid, (int)childpid, i, j);
	    sleep(5);
	}
	printf("Will now wait for my child\n");
	waitpid(childpid, &status, 0);
	printf("Child exited with status %d.\n", WEXITSTATUS(status));
	return 0;

    case 0:
	mypid = getpid();
	for (j = 0; j < runchild; j++) {
	    printf("Hello from the child. I have pid %d. i=%d, j=%d\n", (int)mypid, i, j);
	    sleep(5);
	}
	printf("Child is exiting now!\n");
	return 123;
    }
}
