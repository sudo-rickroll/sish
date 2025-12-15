# For GNU Make and Sun Make

TARGET_NAME = sish
OBJECT_FILES = sish.o builtins.o command.o input.o pipeline.o portability.o redirect.o trace.o

UNAME_S := $(shell uname -s)

CC = cc
CFLAGS = -g -Wall -Werror -Wextra -std=c11

ifeq ($(UNAME_S), SunOS)
	CFLAGS += -D_XOPEN_SOURCE=700 -D__EXTENSIONS__
endif

ifeq ($(UNAME_S), Linux)
	CFLAGS += -D_GNU_SOURCE
endif

all: ${TARGET_NAME}

depend:
	mkdep -- ${CFLAGS} *.c

${TARGET_NAME}: ${OBJECT_FILES}
	$(CC) ${CFLAGS} ${OBJECT_FILES} -o ${TARGET_NAME}

clean:
	rm -- ${TARGET_NAME} ${OBJECT_FILES}
