include config.mk

HDR = cfg.h
CSRC = dfk.c
CXXSRC = cfg.cpp

OBJ = $(CSRC:.c=.o) $(CXXSRC:.cpp=.o)

all: dual-function-keys

$(OBJ): config.mk $(HDR)

dual-function-keys: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o dual-function-keys

clean:
	rm -f dual-function-keys $(OBJ)

install:
	mkdir -p $(PREFIX)/bin
	cp -f dual-function-keys $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/dual-function-keys
	mkdir -p $(PREFIX)/man/man1
	cp dual-function-keys.1 $(PREFIX)/man/man1
	chmod 644 $(PREFIX)/man/man1/dual-function-keys.1

uninstall:
	rm -f $(PREFIX)/bin/dual-function-keys
	rm -f $(PREFIX)/man/man1/dual-function-keys.1

# https://github.com/alex-courtis/arch/blob/b530f331dacaaba27484593a87ca20a9f53ab73f/home/bin/ctags-something
ctags:
	ctags-c $(CPPFLAGS) $(HDR) $(CSRC) $(CXXSRC)

# dual-function-keys.md is pandoc flavoured markdown; please edit that and commit all 3 files
doc: dual-function-keys.md
	pandoc -s --wrap=none -f markdown -t man dual-function-keys.md -o dual-function-keys.1 --shift-heading-level-by=-1
	pandoc -s --wrap=none -f markdown -t commonmark dual-function-keys.md -o README.md

.PHONY: all clean install uninstall ctags
