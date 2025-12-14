CFLAGS = -g -Wall -Werror -Wextra -std=c11
TARGET_NAME = sish
OBJECT_FILES = sish.o builtins.o command.o handlers.o input.o redirect.o trace.o

all: ${TARGET_NAME}

depend:
	mkdep -- ${CFLAGS} *.c

${TARGET_NAME}: ${OBJECT_FILES}
	$(CC) ${CFLAGS} ${OBJECT_FILES} -o ${TARGET_NAME}

clean:
	rm -- ${TARGET_NAME} ${OBJECT_FILES}
