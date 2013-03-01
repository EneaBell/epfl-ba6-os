#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main(void)
{
	int fds[2];
	char buf[20];
	size_t len;

	if (pipe(fds) != 0)
		err(1, "pipe");
	printf("pipe goes from %d -> %d\n", fds[1], fds[0]);

	if (write(fds[1], "test", 4) != 4)
		err(1, "write");

	memset(buf, 0, sizeof(buf));
	len = read(fds[0], buf, sizeof(buf) - 1);
	if ((ssize_t)len == -1)
		err(1, "read");

	printf("read %zd bytes from pipe: %s\n", len, buf);

	printf("closing write side of pipe\n");
	close(fds[1]);

	memset(buf, 0, sizeof(buf));
	len = read(fds[0], buf, sizeof(buf) - 1);
	if ((ssize_t)len == -1)
		err(1, "read");

	printf("read %zd bytes from pipe: %s\n", len, buf);

	close(fds[0]);

	if (pipe(fds) != 0)
		err(1, "pipe");
	printf("pipe goes from %d -> %d\n", fds[1], fds[0]);

	printf("closing read side of pipe\n");
	close(fds[0]);

	printf("about to write to pipe\n");
	if (write(fds[1], "test", 4) != 4)
		err(1, "write");

	printf("wrote to pipe\n");

	return (0);
}
