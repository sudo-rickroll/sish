#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*#include "handlers.h"*/
#include "input.h"

#define SH_NAME "sish$ "

int
main(int argc, char **argv)
{
	char *cmd = {0};

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
		if (tokenize_command(cmd) < 0) {
			err(EXIT_FAILURE, "Error tokenizing string");
		}
	} else {
		char *input = {0};
		size_t input_size;
		ssize_t input_len;
		for (;;) {
			if ((size_t)write(STDOUT_FILENO, SH_NAME, strlen(SH_NAME)) < strlen(SH_NAME)) {
				perror("Writing shell name");

			}
			input_size = 0;
			/* 
			 * getline(3) instead of fgets(3) because input could be
			 * of any length 
			 */
			if((input_len = getline(&input, &input_size, stdin)) == -1){
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

			if (input[0] == '\0') {
				continue;
			}

			if (tokenize_command(input) < 0) {
				perror("Error tokenizing command");
			}

		}
	}

	return EXIT_SUCCESS;

}


