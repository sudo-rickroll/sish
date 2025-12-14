#include <sys/wait.h>

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
#include "redirection.h"
#include "trace.h"

int execute_pipeline(char *input) {
	char *commands[MAX_PIPELINE];
	char *args[MAX_ARGS];
	int cmd_count;
	int pipes[MAX_PIPELINE - 1][2];
	pid_t pids[MAX_PIPELINE];
	int i, status;
	redir_t redir;

	cmd_count = split_pipeline(input, commands);

	if (cmd_count == 1) {
		if (tokenize_command(commands[0], args) < 0) {
			return -1;
		}

		if (args[0] == NULL) {
			return -1;
		}

		if (parse_redirections(args, &redir) < 0) {
			return -1;
		}

		exec_sish(args, &redir);
		return 0;
	}

	/* For multiple pipes */
	for (i = 0; i < cmd_count - 1; i++) {
		if (pipe(pipes[i]) < 0) {
			perror("pipe");
			return -1;
		}
	}

	for (i = 0; i < cmd_count; i++) {
		if (tokenize_command(commands[i], args) < 0) {
			continue;
		}

		if (args[0] == NULL) {
			continue;
		}

		if (parse_redirections(args, &redir) < 0) {
			continue;
		}

		if (trace_enabled) {
			trace_command(args);
		}

		if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "echo") == 0 || strcmp(args[0], "exit") == 0) {
			fprintf(stderr, "Builtins not allowed for now");
			exit_status = 1;

			for (int j = 0; j < cmd_count - 1; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			return -1;
		}

		if ((pids[i] = fork()) < 0) {
			perror("fork");
			return -1;
		}

		if (pids[i] == 0) {
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);

			if (i > 0) {
				if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0) {
					perror("dup2");
					exit(EXIT_FAILURE);
				}
			}

			if (i < cmd_count - 1) {
				if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
					perror("dup2");
					exit(EXIT_FAILURE);
				}
			}

			for (int j = 0; j < cmd_count - 1; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			if (setup_redirection(&redir) < 0) {
				exit(EXIT_FAILURE);
			}

			execvp(args[0], args);
			fprintf(stderr, "%s: command not found\n", args[0]);
			exit(127);
		}
	}

	for (i = 0; i < cmd_count - 1; i++) {
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	for (i = 0; i < cmd_count; i++) {
		if (waitpid(pids[i], &status, 0) < 0) {
			perror("waitpid");
			exit_status = 1;
			return -1;
		}

		if (i == cmd_count - 1) {
			if (WIFEXITED(status)) {
				exit_status = WEXITSTATUS(status);
			} else if (WIFSIGNALED(status)) {
				exit_status = 128 + WTERMSIG(status);
			}
		}
	}

	return 0;
}

