include config.mk

HDR = cfg.h
CSRC = dfk.c
CXXSRC = cfg.cpp

OBJ = $(CSRC:.c=.o) $(CXXSRC:.cpp=.o)

all: dfk

$(OBJ): config.mk $(HDR)

dfk: $(OBJ)

clean:
	rm -f dfk $(OBJ)

install:
	mkdir -p $(PREFIX)/bin
	cp -f dfk $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/dfk
	mkdir -p $(PREFIX)/man/man1
	cp dfk.1 $(PREFIX)/man/man1
	chmod 644 $(PREFIX)/man/man1/dfk.1

uninstall:
	rm -f $(PREFIX)/bin/dfk
	rm -f $(PREFIX)/man/man1/dfk.1

ctags:
	ctags-c $(CPPFLAGS) $(HDR) $(CSRC) $(CXXSRC)

md: dfk.1
	pandoc -s -f man -t commonmark dfk.1 -o gen.md

man: README.md
	pandoc -s -f commonmark -t man README.md -o gen.1

.PHONY: all clean install uninstall ctags md man
