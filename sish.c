#include <stdio.h>
#include <stdlib.h>

#include "input.h"

int
main(int argc, char **argv){
	char *cmd = {0};
	
	if(validate_input(&argc, argv, cmd) < 0) {
		fprintf(stderr, "Unable to setup logging");
		exit(EXIT_FAILURE);
	}

	if(tokenize_command(cmd) < 0){
		fprintf(stderr, "Error tokenizing string");
	}
}


