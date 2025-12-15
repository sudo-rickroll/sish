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
#include "redirect.h"
#include "trace.h"

int 
execute_pipeline_bg(char *input, int background)
{
	char *commands[MAX_PIPELINE];
	char *args[MAX_ARGS];
	int cmd_count, forked_count;
	int pipes[MAX_PIPELINE - 1][2];
	pid_t pids[MAX_PIPELINE];
	int i, j, status;
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

		/* /bin/sh returned 0 for '<file' command */
		if (args[0] == NULL) {
			return 0;
		}

		if (strcmp(args[0], "exit") == 0) {
			if (trace_enabled) {
				trace_command(args);
			}
			exit_sish();
		}

		exec_sish_bg(args, &redir, background);
		return 0;
	}

	forked_count = 0;

	for (i = 0; i < cmd_count; i++) {

		if (i < cmd_count - 1) {
			if (pipe(pipes[i]) < 0) {
				perror("pipe");
				return -1;
			}
		}

		if (tokenize_command(commands[i], args) < 0) {
			pids[i] = -1;
			continue;
		}

		if (args[0] == NULL) {
			pids[i] = -1;
			continue;
		}

		if (parse_redirections(args, &redir) < 0) {
			pids[i] = -1;
			continue;
		}

		if (trace_enabled) {
			trace_command(args);
		}
	
		/* For pipelines with cd or exit, I'm skipping those commands
	 	 * since '/bin/sh' doesn't do anything for them anyways
	 	 */

		if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "exit") == 0)
	       	{
			pids[i] = -1;
			continue;
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

			for (j = 0; j < cmd_count - 1; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			if (setup_redirection(&redir) < 0) {
				exit(EXIT_FAILURE);
			}

			if (strcmp(args[0], "echo") == 0) {
				exit(echo_sish(args));
			}

			execvp(args[0], args);
			fprintf(stderr, "%s: command not found\n", args[0]);
			exit(127);
		}

		forked_count++;
	}

	for (i = 0; i < cmd_count - 1; i++) {
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	if (background) {
		/* Setting last_bg_pid to the last forked process */
		for (i = cmd_count - 1; i >= 0; i--) {
			if (pids[i] != -1) {
				last_bg_pid = pids[i];
				break;
			}
		}
		printf("[%d]\n", last_bg_pid);
		exit_status = 0;
		return 0;
	}

	for (i = 0; i < cmd_count; i++) {
		if (pids[i] == -1) {
			continue;
		}

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
