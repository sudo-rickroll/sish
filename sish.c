#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "command.h"
#include "globals.h"
#include "input.h"
#include "pipeline.h"
#include "redirect.h"

int exit_status = 0;
pid_t last_bg_pid = 0;
FILE *trace_file = NULL;
int trace_enabled = 0;

void
set_shell_env(char *argv_0)
{
	char path[BUFSIZ];

	if (realpath(argv_0, path) == NULL) {
		setenv("SHELL", argv_0, 1);
		return;
	}

	setenv("SHELL", path, 1);
}


int
main(int argc, char **argv)
{
	char *cmd = {0};

	(void)setprogname(argv[0]);

	set_shell_env(argv[0]);

	if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
		err(EXIT_FAILURE, "Unable to ignore SIGINT");
	}
	
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
		err(EXIT_FAILURE, "Unable to ignore SIGQUIT");
	}

	if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
		err(EXIT_FAILURE, "Unable to ignore SIGTSTP");
	}
	
	if (validate_input(&argc, &argv, &cmd) < 0) {
		err(EXIT_FAILURE, "Unable to setup logging");
	}

	if (cmd != NULL) {

		if (execute_pipeline(cmd) < 0) {
			exit(exit_status == 0 ? EXIT_FAILURE : exit_status);
		}

		exit(exit_status);

	} else {
		char *input = {0};
		size_t input_size;
		ssize_t input_len;
		for (;;) {
			printf("%s$ ", getprogname());
			fflush(stdout);
			input_size = 0;
			/* 
			 * getline(3) instead of fgets(3) because input could be
			 * of any length 
			 */
			if((input_len = getline(&input, &input_size, stdin)) == -1){
				free(input);
				/* for ctrl+d */
				if (feof(stdin)) {
					printf("\n");
					exit(EXIT_SUCCESS);
				} else {
					err(EXIT_FAILURE, "Error reading input.");
				}
			}
			
			if (input_len > 0 && input[input_len - 1] == '\n') {
				input[input_len - 1] = '\0';
				input_len--;
			}

			if (input_len == 0 || input[0] == '\0') {
				continue;
			}

			execute_pipeline(input);				
		}
		free(input);
		return exit_status;
	}

	return EXIT_SUCCESS;

}


