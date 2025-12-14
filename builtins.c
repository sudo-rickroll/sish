#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"

int
cd_sish(char *dir){
	(void)dir;
	return 0;
}

int
echo_sish(char *word){
	(void)word;
	return 0;
}

void
exit_sish(){
	exit(EXIT_SUCCESS);
}
