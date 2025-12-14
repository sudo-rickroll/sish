#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "input.h"
#include "trace.h"

static char arg_storage[MAX_ARGS][BUFSIZ];
extern int exit_status;

int
validate_input(int *argc, char ***argv, char **cmd)
{
	int ch;
	while ((ch = getopt(*argc, *argv, "xc:")) != -1) {
		switch (ch) {
			case 'x':
				if(enable_trace(stderr) < 0){
					return -1;
				}
				break;
			case 'c':
				*cmd = optarg;
				break;
			case '?':
			default:
				usage();
		}
	}
	*argc -= optind;
	*argv += optind;

	return 0;		
}

int
tokenize_command(char *cmd, char **args) 
{
	int arg_count = 0;
	char *token;
	
	/* Copied from example in man page of strtok(3) */
	for((token = strtok(cmd, " \t")); token; (token = strtok(NULL, " \t")), arg_count++){
		if (arg_count >= MAX_ARGS - 1) {
			fprintf(stderr, "Arguments have a limit of %d", MAX_ARGS - 1);
			return -1;
		}

		args[arg_count] = arg_storage[arg_count];

		if (expand_token(token, arg_count) < 0) {
			fprintf(stderr, "Error interpreting %s", token);
			return -1;
		}
	}

	args[arg_count] = NULL;

	if (arg_count == 0) {
		return -1;
	}
	return 0;
}

int
expand_token(char *token, int index){

	char *env = {0};

	if (strcmp(token, "$$") == 0) {
		if (snprintf(arg_storage[index], BUFSIZ, "%d", getpid()) < 0) {
			perror("snprintf pid");
			return -1;
		}
		return 0;
	}

	if (strcmp(token, "$?") == 0) {
		if(snprintf(arg_storage[index], BUFSIZ, "%d", exit_status) < 0){
			perror("snprintf status");
			return -1;
		}
		return 0;
	}

	if (*token == '$') {
		if ((env = getenv(token + 1)) == NULL) {
			arg_storage[index][0] = '\0';
			return 0;
		}
		if (strlcpy(arg_storage[index], env, BUFSIZ) >= BUFSIZ) {
			fprintf(stderr, "Env too big\n");
			return -1;
		}
	}

	if (strlcpy(arg_storage[index], token, BUFSIZ) >= BUFSIZ) {
		fprintf(stderr, "Token too big");
		return -1;
	}

	return 0;
}






