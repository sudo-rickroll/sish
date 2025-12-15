#include <sys/wait.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "command.h"
#include "globals.h"
#include "redirect.h"
#include "trace.h"

/*
 * Note: Exit codes used here are from 
 * https://tldp.org/LDP/abs/html/exitcodes.html
 */
void
exec_sish_bg(char **args, redir_t *redir, int background)
{
	pid_t pid;
	int status;
	int saved_stdin, saved_stdout;
	int builtin_ret;

	saved_stdin = -1;
	saved_stdout = -1;
	builtin_ret = 0;

	trace_command(args);

	if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "echo") == 0) {
		if (background) {
			fprintf(stderr, "Cannot background builtin commands\n");
			exit_status = 1;
			return;
		}

		if (redir->in_file != NULL || redir->out_file != NULL) {
			saved_stdin = dup(STDIN_FILENO);
			saved_stdout = dup(STDOUT_FILENO);
			if (setup_redirection(redir) < 0) {
				if (saved_stdin >= 0) {
					close(saved_stdin);
				}
				if (saved_stdout >= 0) {
					close(saved_stdout);
				}
				exit_status = 1;
				return;
			}
		}

		if (strcmp(args[0], "cd") == 0) {
			builtin_ret = cd_sish(args);
		} else if (strcmp(args[0], "echo") == 0) {
			builtin_ret = echo_sish(args);
		}

		if (saved_stdin >= 0) {
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout >= 0) {
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}

		exit_status = builtin_ret;
		return;
	}

	if (strcmp(args[0], "exit") == 0) {
		exit_sish();
	}

	if ((pid = fork()) == -1) {
		perror("fork");
		exit_status = -1;
		return;
	}

	if (pid == 0) {
		/* Restoring signals for misc commands */
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);

		if (setup_redirection(redir) < 0) {
			exit(EXIT_FAILURE);
		}

		execvp(args[0], args);
		
		fprintf(stderr, "%s: command not found\n", args[0]);

		exit(127);
	}

	if (background) {
		last_bg_pid = pid;
		printf("[%d]\n", last_bg_pid);
		exit_status = 0;
		return;
	}

	if (waitpid(pid, &status, 0) == -1) {
		perror("waitpid");
		exit_status = -1;
		return;
	}

	if (WIFEXITED(status)) {
		exit_status = WEXITSTATUS(status);
		return;
	}

	if (WIFSIGNALED(status)) {
		exit_status = 128 + WTERMSIG(status);
	}
}
