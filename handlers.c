#include <stdio.h>

#include "handlers.h"

void
sig_ignore(int signo){
	(void)signo;
	printf("Signals are currently ignored. Try again later\n");
	return;
}
