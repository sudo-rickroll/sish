#ifndef INPUT_H

#define INPUT_H

#define MAX_ARGS 64

int
validate_input(int *, char ***, char **);

int
tokenize_command(char *, char**);

int
expand_token(char *, int);

int
split_pipeline(char *, char **);

#endif


