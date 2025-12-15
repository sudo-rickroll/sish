#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "redirect.h"

int
parse_redirections(char **args, redir_t *redir)
{
	int i, j;

	redir->in_file = NULL;
	redir->out_file = NULL;
	redir->append = 0;

	for (i = 0, j = 0; args[i] != NULL; i++) {
		if (strcmp(args[i], "<") == 0) {
			if (args[i + 1] == NULL) {
				fprintf(stderr, "Missing input file\n");
				return -1;
			}
			redir->in_file = args[i + 1];
			i++;
		} else if (strcmp(args[i], ">") == 0) {
			if (args[i + 1] == NULL) {
				fprintf(stderr, "Missing output file\n");
				return -1;
			}
			redir->out_file = args[i + 1];
			redir->append = 0;
			i++;
		} else if (strcmp(args[i], ">>") == 0) {
			if (args[i + 1] == NULL) {
				fprintf(stderr, "Missing output file\n");
				return -1;
			}
			redir->out_file = args[i + 1];
			redir->append = 1;
			i++;
		} else {
			args[j++] = args[i];
		}
	}
	args[j] = NULL;

	return 0;
}

int
setup_redirection(redir_t *redir)
{
	int fd;

	if (redir->in_file != NULL) {
		if ((fd = open(redir->in_file, O_RDONLY)) < 0) {
			perror("open input");
			return -1;
		}
		if (dup2(fd, STDIN_FILENO) < 0) {
			perror("dup2 stdin");
			close(fd);
			return -1;
		}
		close(fd);
	}

	if (redir->out_file != NULL) {
		int flags;

		flags = O_WRONLY | O_CREAT;
		if (redir->append) {
			flags |= O_APPEND;
		} else {
			flags |= O_TRUNC;
		}

		if ((fd = open(redir->out_file, flags, 0666)) < 0) {
			perror("open output");
			return -1;
		}
		if (dup2(fd, STDOUT_FILENO) < 0) {
			perror("dup2 stdout");
			close(fd);
			return -1;
		}
		close(fd);
	}

	return 0;
}
