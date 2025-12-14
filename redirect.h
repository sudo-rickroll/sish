#ifndef REDIRECT_H
#define REDIRECT_H

typedef struct {
	char *in_file;
	char *out_file;
	int append;
} redir_t;

int
setup_redirection(redir_t *redir);

int
parse_redirections(char **args, redir_t *redit);

#endif
