#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static const char *text1 =
"001\n"
"002\n"
"003\n"
"004\n";

static const char *text2 =
"aaa\n"
"bbb\n"
"ccc\n"
"eee\n";

static void
readfromfile(int fd)
{
	char buf[5];

	memset(buf, 0, sizeof(buf));
	if (read(fd, buf, 4) != 4)
		err(1, "read");

	printf("read from fd %d: %s", fd, buf);
}

static void
writetofile(const char *name, const char *text)
{
	int fd;

	fd = open(name, O_RDWR|O_CREAT, 0666);
	if (fd < 0)
		err(1, "open");

	if (write(fd, text, strlen(text)) != strlen(text))
		err(1, "write");

	close(fd);
}

int
main(void)
{
	int fd1, fd2;
	const char *name1 = "/tmp/test1", *name2 = "/tmp/test2";

	writetofile(name1, text1);
	writetofile(name2, text2);

	fd1 = open(name1, O_RDONLY);
	if (fd1 < 0)
		err(1, "open(%s)", name1);

	printf("file %s has fd %d\n", name1, fd1);

	fd2 = open(name2, O_RDONLY);
	if (fd2 < 0)
		err(1, "open(%s)", name2);

	printf("file %s has fd %d\n", name2, fd2);

	readfromfile(fd1);
	readfromfile(fd2);

	printf("dupping fd %d to fd %d\n", fd1, fd2);
	dup2(fd1, fd2);

	readfromfile(fd1);
	readfromfile(fd2);

	return (0);
}
