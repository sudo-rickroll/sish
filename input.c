#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "input.h"
#include "trace.h"

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

		args[arg_count] = token;
	}

	args[arg_count] = NULL;

	if (arg_count == 0) {
		return -1;
	}
	return 0;
}


