.POSIX:

include config.mk

HDR = config.h
SRC = dfk.cpp
OBJ = $(SRC:.cpp=.o)

all: dfk

.cpp.o:
	$(CC) -c $(CFLAGS) $<

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
	generate-ctags-c $(INCS) $(HDR) $(SRC)

.PHONY: all clean install uninstall ctags
