#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/param.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


static int error;
static int inout[2];

typedef int (*builtin_cmd)(int, char **);

struct builtin {
	const char *name;
	builtin_cmd func;
};
#define	BIN(n)	{ #n, builtin_ ## n }


int
builtin_cd(int argc, char **argv)
{
	/* body of cd builtin command */
	/* add your code here */
	return (0);
}

int
builtin_exit(int argc, char **argv)
{
	/* body of exit builtin command */
	/* add your code here */
	return (0);
}

int
builtin_status(int argc, char **argv)
{
	/* body of the status builtin command */
	/* add your code here */
	return (0);
}

static struct builtin builtins[] = {
	BIN(cd),
	BIN(exit),
	BIN(status),
	{ NULL }
};

/*
 * run_builtin handles setting up the handlers for your builtin commands
 *
 * It accepts the array of strings, and tries to match the first argument
 * with the builtin mappings defined in the builtins[].
 * Returns 0 if it did not manage to find builtin command, or 1 if args
 * contained a builtin command
 */
static int
run_builtin(char **args)
{
	int argc;
	struct builtin *b;

	for (b = builtins; b->name != NULL; b++) {
		if (strcmp(b->name, args[0]) == 0) {
			for (argc = 0; args[argc] != NULL; argc++)
				/* NOTHING */;
			error = b->func(argc, args);
			return (1);
		}
	}

	return (0);
}

/* add your code here */

/*
 * Takes a pointer to a string pointer and advances this string pointer
 * to the word delimiter after the next word.
 * Returns a pointer to the beginning of the next word in the string,
 * or NULL if there is no more word available.
 */
static char *
parseword(char **pp)
{
	char *p = *pp;
	char *word;

	for (; isspace(*p); p++)
		/* NOTHING */;

	word = p;

	for (; strchr(" \t;&|><\n", *p) == NULL; p++)
		/* NOTHING */;

	*pp = p;
	return (p != word ? word : NULL);
}

static void
process(char *line)
{
	int ch, ch2;
	char *p, *word;
	char *args[100], **narg;
	int pip[2];
	int fd, mode;

	p = line;

newcmd:
	inout[0] = STDIN_FILENO;
	inout[1] = STDOUT_FILENO;

newcmd2:
	narg = args;
	*narg = NULL;

	for (; *p != 0; p != line && p++) {
		word = parseword(&p);

		ch = *p;
		*p = 0;

		/*
		printf("parseword: '%s', '%c', '%s'\n", word, ch, p + 1);
		*/

		if (word != NULL) {
			*narg++ = word;
			*narg = NULL;
		}

nextch:
		/*
		 * Here you should put your code for processing the commands
		 * Up to this point, pointer word points to the next word,
		 * ch points to the first character after the word
		 * You should process according to what is in the ch.
		 * For example, use switch(). Next example will skip whitespaces
		 * and detect the redirection of the standard output.
		 *

		switch (ch) {
		case ' ':
		case '\t': break;
		case '>':
			warn('Ah, we have redirection!');
			break;
		case '\n':
			RUN_COMMAND();
			break;
		default:
		}

		 *
		 * The previous example is broken in many ways, though,
		 * so you should not use it verbatim.
		 *
		 */

		/* add your code here */
		;
	}
}

int
main(void)
{
	char cwd[MAXPATHLEN+1];
	char line[1000];
	char *res;

	for (;;) {
		getcwd(cwd, sizeof(cwd));
		printf("%s %% ", cwd);

		res = fgets(line, sizeof(line), stdin);
		if (res == NULL)
			break;

		process(line);
	}

	return (error);
}
