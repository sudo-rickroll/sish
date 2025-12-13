#include <stdio.h>

#include "trace.h"

int
enable_trace(FILE *file)
{
	(void)file;
	return 0;
}

void
usage(void)
{
	printf("Not like that");
}
