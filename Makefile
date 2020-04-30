include config.mk

HDR = cfg.h
SRC = dfk.c cfg.cpp
OBJ = dfk.o cfg.o

all: dfk

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

.cpp.o:
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $<

$(OBJ): $(HDR) config.mk

dfk: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

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
	ctags-c $(CPPFLAGS) $(HDR) $(SRC)

doc:
	pandoc -f man -t gfm dfk.1 > README.md

.PHONY: all clean install uninstall ctags
