#include <sys/wait.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "command.h"
#include "globals.h"
#include "trace.h"

/*
 * Note: Exit codes used here are from 
 * https://tldp.org/LDP/abs/html/exitcodes.html
 */
void
exec_sish(char **args) {
	pid_t pid;
	int status;

	trace_command(args);

	if (strcmp(args[0], "cd") == 0) {
		exit_status= cd_sish(args);
		return;
	}

	if (strcmp(args[0], "echo") == 0) {
		exit_status = echo_sish(args);
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

		execvp(args[0], args);
		
		perror("exec");
		exit_status = 127;

		exit(EXIT_FAILURE);
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

