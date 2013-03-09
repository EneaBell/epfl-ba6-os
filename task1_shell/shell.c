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
//      08.03.2013 - 1.1 - External command
//
//
//  Rule of Thumb:
//      file_descriptor -> As soon as you dup, you close the old one.
//      pipe -> If you fork, close the output from the parent side.
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

int builtin_cd(int argc, char **argv) {
	return (argc > 1) ? chdir(argv[1]) : 0;
}

int builtin_exit(int argc, char **argv) {
	exit(error);
	return (1);
}

int builtin_status(int argc, char **argv) {
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
static int run_builtin(char **args) {
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

static int run_command2(char **args, int pipe_enable) {
	
    int fd;
    int pip[2];
    int childpid;
    
    // No command
    if (args[0] == NULL) {
        restore_std();
        return 0;
    }
    
    if (pipe_enable) {
        if (pipe(pip) != 0) {
            err(1, "Couldn't open pipe");
        }
        
    } else if (run_builtin(args)) {
        return 1;
    }
    
    childpid = fork();
    
    switch (childpid) {
        case -1: //
            err(1, "Unable to fork.");
            break;
            
        case 0: // child: run command and exit

            fflush(stdout);
            
            if (pipe_enable && (inout[1] == STDOUT_FILENO)) {
                fflush(stdout);
                dup2(pip[1], STDOUT_FILENO);
                close(pip[1]);
            }
            
            if (!pipe_enable || !run_builtin(args)) {
                // execvp
                //   -> v = args inside an array
                //   -> p = check PATH to find the command
                execvp(args[0], args);
                err(1, "Error executing file");
            }
    
            restore_std();
    
            fflush(stdout);
            exit(error);
            break;
            
        default: // parent
            
            restore_std();
            
            if (pipe_enable) { // run next command and stay alive
                fd = dup(inout[0]);
                close(inout[0]);
                inout[0] = fd;
                
                dup2(pip[0], STDIN_FILENO);
                close(pip[0]);
                close(pip[1]); // Has to be closed from parent side (rule of thumb)
                
            } else { // wait for the child
                waitpid(childpid, &error, 0);
            }
            break;
    }
    
    *args = NULL;
    
	return childpid;
}

static int run_command(char **args) {
    return run_command2(args, 0);
}

static int run_pipe(char **args) {
    return run_command2(args, 1);
}

void quit_process(int sig_no) {
	char* str = "\r\n";
	
	if (sig_no == SIGINT) {
		write(inout[1], str, strlen(str));
	}
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
	int fd, mode;
    
	p = line;

//newcmd:
  // Set the standard stdin & stdout
	inout[0] = STDIN_FILENO;
	inout[1] = STDOUT_FILENO;
    
//newcmd2:
	narg = args;
	*narg = NULL;
    
	for (; *p != 0; p != line && p++) {
		word = parseword(&p);
        
		ch  = *p;
    ch2 = *(p + 1);
		*p  = 0;
        
		/* printf("parseword: '%s', '%c', '%s'\n", word, ch, p + 1); */
        
		if (word != NULL) {
			*narg++ = word;
			*narg = NULL;
		}
    
//nextch:
        switch (ch) {
            // redirection: cmd > file
            case '>':
                p++;
                word = parseword(&p); // file
                *p = 0;
                
                // Backup stdout
                inout[1] = dup(inout[1]);
                
                // TODO Extract mode from cwd
                fd = open(word, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd < 0) err(1, "%s", word);
                dup2(fd, STDOUT_FILENO);
                close(fd);
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
                    
                    if (run_pipe(args) > 0) {
                        // reinitialize args
                        narg = args;
                        *narg = NULL;
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
                    
                } else { // background: cmd1 &
										++p;
										if (fork() == 0)
											execvp(args[0], args);
                }
                break;
                
            case '\n':
                if (run_command(args) != 0) {
                    // reinitialize args
                    narg = args;
                    *narg = NULL;
                }
                break;
            
            case ' ':
            case '\t':
            default:
                break;
        }
	}
    
		/* 
			IF THIS IS PRESENT, THE BACKGROUND JOBS DO NOT WORK PROPERLY. 
			AND IF COMMENTED, THE ^C for the command yes > /dev/null does not work...
			FUCK YOU THEN PROGRAM :D
		*/
    // if there is still a command to run
    if (run_command(args) != 0) {
        // reinitialize args
        narg = args;
        *narg = NULL;
    }
}

int main(void)
{
	char cwd[MAXPATHLEN+1];
	char line[1000];
  char *res;
    
	for (;;) {
		getcwd(cwd, sizeof(cwd));
		printf("%s %% ", cwd);
    
		// Catches ^C and do handler method quit_process(int)
		signal(SIGINT, &quit_process);
    
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
