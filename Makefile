.POSIX:

include config.mk

HDR = config.h
SRC = dfk.c
OBJ = ${SRC:.c=.o}

all: dfk

options:
	@echo dfk build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: ${HDR} config.mk

dfk: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f dfk ${OBJ}

ctags:
	generate-ctags-c ${CFLAGS} ${HDR} ${SRC}

.PHONY: all options clean ctags
