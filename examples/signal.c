#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MSGLEN 1000

void sig_hdlr(int sig_num)
{
    static const char* msgs[] = {
	"Caught ^C\n",
	"Caught SIGUSR1\n",
	"Unknown signal!\n"
    };
    const char *msg;

    switch(sig_num) {
    case SIGINT:
	msg = msgs[0];
        break;
    case SIGUSR1:
        msg = msgs[1];
        break;
    default:
	msg = msgs[2];
	break;
    }

    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(int argc, char* argv[])
{
    pid_t pid;

    pid = getpid();
    printf("Press ^C or send signals to %d with `kill'. Terminate with `kill -9 %d'\n", (int)pid, (int)pid);

    signal(SIGINT, &sig_hdlr);
    signal(SIGUSR1, &sig_hdlr);

    while(1) sleep(1000);

    return 0;
}
