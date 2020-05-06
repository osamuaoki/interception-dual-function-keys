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

# https://github.com/alex-courtis/arch/blob/b530f331dacaaba27484593a87ca20a9f53ab73f/home/bin/ctags-something
ctags:
	ctags-c $(CPPFLAGS) $(HDR) $(CSRC) $(CXXSRC)

# dfk.md is pandoc flavoured markdown; please edit that and commit all 3 files
doc: dfk.md
	pandoc -s --wrap=none -f markdown -t man dfk.md -o dfk.1 --shift-heading-level-by=-1
	pandoc -s --wrap=none -f markdown -t commonmark dfk.md -o README.md

.PHONY: all clean install uninstall ctags
