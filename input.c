#include <unistd.h>

#include "input.h"
#include "trace.h"

int
validate_input(int *argc, char ***argv, char **cmd)
{
	int ch;
	while ((ch = getopt(*argc, *argv, "xc:")) != -1) {
		switch (ch) {
			case 'x':
				if(enable_trace(stderr) < 0){
					return -1;
				}
				break;
			case 'c':
				*cmd = optarg;
				break;
			case '?':
			default:
				usage();
		}
	}
	*argc -= optind;
	*argv += optind;

	return 0;		
}

int
tokenize_command(char *cmd) 
{
	(void)cmd;
	return 0;
}


