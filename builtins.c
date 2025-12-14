#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"

int
cd_sish(char *dir)
{
	struct passwd *pw;

	if (dir == NULL) {
		if((dir = getenv("HOME")) == NULL) {
			if((pw = getpwuid(getuid())) == NULL) {
				perror("getpwuid");
				return 1;
			}
			dir = pw->pw_dir;
		}
	}

	if (chdir(dir) != 0) {
	       perror("chdir");
	       return 1;
	}     
	
	return 0;
}

int
echo_sish(char **words)
{
	int pointer;

	for(pointer = 1; words[pointer] != NULL; pointer++){
		printf("%s", words[pointer]);
		if (words[pointer + 1] != NULL) {
				printf(" ");
		}
	}

	printf("\n");

	return 0;
}

void
exit_sish()
{
	exit(EXIT_SUCCESS);
}
