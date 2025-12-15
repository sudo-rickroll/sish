#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "portability.h"
#include "trace.h"

int
enable_trace(FILE *file)
{
	if (file == NULL) {
		return -1;
	}
	trace_file = file;
	trace_enabled = 1;
	return 0;
}

void
trace_command(char **args)
{
	int i;

	if (!trace_enabled || trace_file == NULL || args == NULL) {
		return;
	}

	fprintf(trace_file, "+");
	for (i = 0; args[i] != NULL; i++) {
		fprintf(trace_file, " %s", args[i]);
	}
	fprintf(trace_file, "\n");
	fflush(trace_file);
}

void
usage(void)
{

	(void)fprintf(stderr, "usage: %s [-x] [-c command]\n",
	    getprogname());
	exit(EXIT_FAILURE);
}
