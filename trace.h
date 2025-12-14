#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>

int
enable_trace(FILE *);

void
trace_command(char **);

void
usage(void);

#endif


