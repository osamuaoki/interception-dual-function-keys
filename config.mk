PREFIX = /usr/local

INCS = -I. -I/usr/include

CPPFLAGS = $(INCS)

COMPFLAGS = -pedantic -Wall -Wextra -O3
CFLAGS = $(COMPFLAGS) -std=c99
CXXFLAGS = $(COMPFLAGS) -std=c++2a # todo: revert to c++11 after removing the static struct initalisers

LDFLAGS = -lstdc++ -lyaml-cpp

CC = cc

