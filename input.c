#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "globals.h"
#include "input.h"
#include "pipeline.h"
#include "trace.h"


static char arg_storage[MAX_ARGS][BUFSIZ];

int
validate_input(int *argc, char ***argv, char **cmd)
{
	int ch;

	while ((ch = getopt(*argc, *argv, "xc:")) != -1) {
		switch (ch) {
		case 'x':
			if (enable_trace(stderr) < 0) {
				return -1;
			}
			break;
		case 'c':
			*cmd = optarg;
			break;
		case '?':
		default:
			usage();
			return -1;
		}
	}
	*argc -= optind;
	*argv += optind;

	return 0;		
}

int
tokenize_command(char *cmd, char **args) 
{
	int arg_count;
	char *p;
	char token[BUFSIZ];
	int index;

	arg_count = 0;
	p = cmd;

	while (*p != '\0') {
		while (*p == ' ' || *p == '\t') {
			p++;
		}

		if (*p == '\0') {
			break;
		}

		index = 0;
		
		/* For >> */
		if (*p == '>' && *(p + 1) == '>') {
			token[0] = '>';
			token[1] = '>';
			token[2] = '\0';
			p += 2;
		} else if (*p == '<' || *p == '>' || *p == '|') {
			token[0] = *p;
			token[1] = '\0';
			p++;
		} else {
			while (*p != '\0' && *p != ' ' && *p != '\t' &&
			    !(*p == '<' || *p == '>' || *p == '|')) {
				token[index++] = *p++;
			}
			token[index] = '\0';
		}

		if (arg_count >= MAX_ARGS - 1) {
			fprintf(stderr, "Arguments have a limit of %d",
			    MAX_ARGS - 1);
			return -1;
		}

		args[arg_count] = arg_storage[arg_count];

		if (expand_token(token, arg_count) < 0) {
			fprintf(stderr, "Error interpreting %s", token);
			return -1;
		}
		arg_count++;
	}

	args[arg_count] = NULL;

	if (arg_count == 0) {
		return -1;
	}
	return 0;
}

int
expand_token(char *token, int index)
{
	char *env;

	env = NULL;

	if (strcmp(token, "$!") == 0) {
		if (snprintf(arg_storage[index], BUFSIZ, "%d",
		    last_bg_pid) < 0) {
			perror("snprintf bg pid");
			return -1;
		}
		return 0;
	}

	if (strcmp(token, "$$") == 0) {
		if (snprintf(arg_storage[index], BUFSIZ, "%d",
		    getpid()) < 0) {
			perror("snprintf pid");
			return -1;
		}
		return 0;
	}

	if (strcmp(token, "$?") == 0) {
		if (snprintf(arg_storage[index], BUFSIZ, "%d",
		    exit_status) < 0) {
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
		return 0;
	}

	if (strlcpy(arg_storage[index], token, BUFSIZ) >= BUFSIZ) {
		fprintf(stderr, "Token too big");
		return -1;
	}

	return 0;
}

int
split_pipeline(char *input, char *commands[])
{
	int count;
	char *token;
	char *last;

	count = 0;
	last = input;

	for ((token = strtok_r(last, "|", &last)); token && count < MAX_PIPELINE;
	    (token = strtok_r(NULL, "|", &last)), count++) {
		commands[count] = token;
	}

	commands[count] = NULL;
	return count;
}

int
check_background(char *input)
{
	size_t len;
	char *p;

	len = strlen(input);
	if (len == 0) {
		return 0;
	}

	/* Find the last non-whitespace character */
	p = input + len - 1;
	while (p >= input && (*p == ' ' || *p == '\t')) {
		p--;
	}

	/* Check if it's '&' */
	if (p >= input && *p == '&') {
		/* Remove the '&' and trailing whitespace */
		*p = '\0';
		p--;
		while (p >= input && (*p == ' ' || *p == '\t')) {
			*p = '\0';
			p--;
		}
		return 1;
	}

	return 0;
}
