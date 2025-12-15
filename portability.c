#include<string.h>

#include "portability.h"

char *progname = NULL;

void
setprogname(const char *name)
{
	const char *p;

	p = strchr(name, '/');
	if (p != NULL) {
		progname = strdup(p+1);
	} else {
		progname = strdup(name);
	}
}

const char*
getprogname() {
	return progname ? progname : "sish";
}

