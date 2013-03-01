#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char **argv)
{
	int i;
	char *style, *cmd, *param;

	printf("%d args:", argc);
	for (i = 0; i < argc; i++)
		printf(" %s", argv[i]);
	printf("\n");

	if (argc != 4) {
		fprintf(stderr, "usage: %s style cmd param\n", argv[0]);
		return (1);
	}

	style = argv[1];
	cmd = argv[2];
	param = argv[3];

	printf("testing %s\n", style);

	if (strcmp(style, "execl") == 0) {

		execl(cmd, cmd, param , NULL);

	} else if (strcmp(style, "execv") == 0) {
		char *args[] = {
			"arg0",
			"123",
			"456",
			NULL
		};

		execv(cmd, args);

	} else if (strcmp(style, "execvp") == 0) {
		char *args[] = {
			"arg0",
			"123",
			"456",
			NULL
		};

		execvp(cmd, args);

	} else {
		fprintf(stderr, "unknown style!\n");
		return (1);
	}

	warn("after %s", style);
	return (1);
}
