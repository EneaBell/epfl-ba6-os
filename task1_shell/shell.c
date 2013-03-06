/* ---------------------------------------------------------------------------
//   shell.c
// ---------------------------------------------------------------------------
//  Project:
//      Musso Claire    (205784)
//      Dieulivol David (185078)
//      Denoréaz Thomas (183785)
//
//  Versions:
//      07.03.2013 - 1.0 - Initial release
//
// ------------------------------------------------------------------------ */

#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

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


int builtin_cd(int argc, char **argv)
{
	return (argc > 1) ? chdir(argv[1]) : 0;
}

int builtin_exit(int argc, char **argv)
{
	exit(error);
	return (1);
}

int builtin_status(int argc, char **argv)
{
	printf("%d\n", error);
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
static int run_builtin(char **args)
{
	int argc;
	struct builtin *b;
    
	for (b = builtins; b->name != NULL; b++) {
		if (strcmp(b->name, args[0]) == 0) {
			for (argc = 0; args[argc] != NULL; argc++);
			error = b->func(argc, args);
			return (1);
		}
	}
    
	return (0);
}

static void restore_std() {

    fflush(stdout);
    fflush(stderr);
    
    // restore stdout
    if (inout[1] != STDOUT_FILENO) {
        dup2(inout[1], STDOUT_FILENO);
        close(inout[1]);
        inout[1] = STDOUT_FILENO;
    }
    
    // restore stdin
    if (inout[0] != STDIN_FILENO) {
        //fpurge(stdin);
        dup2(inout[0], STDIN_FILENO);
        close(inout[0]);
        inout[0] = STDIN_FILENO;
    }
}

static int run_command(char **args)
{
    // No command
    if (args[0] == NULL) {
        return 0;
    }
    
	if (!run_builtin(args)) {
        // TODO launch command using exec
        printf("TODO launch command (%s) using exec\n", args[0]);
        error = 1;
    }
    
    restore_std();
    
    *args = NULL;
    
	return 0;
}

/* add your code here */


/*
 * Takes a pointer to a string pointer and advances this string pointer
 * to the word delimiter after the next word.
 * Returns a pointer to the beginning of the next word in the string,
 * or NULL if there is no more word available.
 */
static char* parseword(char **pp)
{
	char *p = *pp;
	char *word;
    
	for (; isspace(*p); p++);
    
	word = p;
    
	for (; strchr(" \t;&|><\n", *p) == NULL; p++);
    
	*pp = p;
	return (p != word ? word : NULL);
}

static void process(char *line)
{
	int ch, ch2;
	char *p, *word;
	char *args[100], **narg;
	int pip[2];
	int fd, mode;
    int childpid;
    
	p = line;
    
newcmd:
    // Set the standard stdin & stdout
	inout[0] = STDIN_FILENO;
	inout[1] = STDOUT_FILENO;
    
newcmd2:
	narg = args;
	*narg = NULL;
    
	for (; *p != 0; p != line && p++) {
		word = parseword(&p);
        
		ch  = *p;
        ch2 = *(p+1);
		*p  = 0;
        
		/*
         printf("parseword: '%s', '%c', '%s'\n", word, ch, p + 1);
         */
        
		if (word != NULL) {
			*narg++ = word;
			*narg = NULL;
		}
        
nextch:
        switch (ch) {
            // redirection: cmd > file
            case '>':
                p++;
                word = parseword(&p); // file
                *p = 0;
                
                // Backup stdout
                inout[1] = dup(inout[1]);
                
                // TODO Extract mode from cwd
                // TODO Close fd after
                fd = open(word, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd < 0) err(1, "%s", word);
                dup2(fd, STDOUT_FILENO);
                break;
                
            // redirection: cmd < file
            case '<':
                p++;
                word = parseword(&p); // file
                *p = 0;
                
                // Backup stdin
                inout[0] = dup(inout[0]);
                
                fd = open(word, O_RDONLY);
                if (fd < 0) err(1, "%s", word);
                dup2(fd, STDIN_FILENO);
                close(fd);
                break;
            
            // pipe: cmd1 | cmd2
            // conditional execution: cmd1 || cmd2
            case '|':
                if (ch2 == '|') { // conditional execution: cmd1 || cmd2
                    ++p; // shift to pass ch2
                    
                    // TODO conditional execution OR
                    printf("TODO conditional execution OR\n");
                    
                } else { // pipe: child | parent

                    if (pipe(pip) != 0) {
                        err(1, "Couldn't open pipe");
                        
                    } else {
                        
                        childpid = fork();
                        
                        switch (childpid) {
                            case -1: //
                                err(1, "Unable to fork.");
                                
                            case 0: // child: run first command and exit
                                if (inout[1] == STDOUT_FILENO) {
                                    fflush(stdout);
                                    dup2(pip[1], STDOUT_FILENO);
                                }
                                
                                run_command(args);
                                
                                fflush(stdout);
                                exit(error);
                                break;
                                
                            default: // parent: run next command and stay alive
                                restore_std();
                                
                                inout[0] = dup(inout[0]);
                                dup2(pip[0], STDIN_FILENO);
                                close(pip[0]);
                                
                                // reinitialize args
                                narg = args;
                                *narg = NULL;
                                break;
                        }

                    }
                }
                break;
                
            // background: cmd1 & cmd2
            // conditional execution: cmd1 && cmd2
            case '&':
                if (ch2 == '&') { // conditional execution: cmd1 && cmd2
                    ++p; // shift to pass ch2
                    
                    // TODO conditional execution AND
                    printf("TODO conditional execution AND\n");
                    
                } else { // background: cmd1 & cmd2
                    // TODO background execution
                    printf("TODO background execution\n");
                    
                }
                break;
                
            case '\n':
                run_command(args);
                break;
            
            case ' ':
            case '\t':
            default:
                break;
        }
	}
    
    // if there is still a command to run
    run_command(args);
}

int main(void)
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

/* ---------------------------------------------------------------------------
// ----- End of File ---------------------------------------------------------
// ------------------------------------------------------------------------ */
