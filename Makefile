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

# from https://www.topbug.net/blog/2012/03/17/generate-ctags-files-for-c-slash-c-plus-plus-source-files-and-all-of-their-included-header-files/
# can be properly integrated with make; the -M output is for make!
ctags:
	${CC} -M ${CFLAGS} ${HDR} ${SRC} | \
		sed -e 's/[\\ ]/\n/g' | \
		sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' | \
		ctags -L - --kinds-C=+pl

.PHONY: all options clean ctags
