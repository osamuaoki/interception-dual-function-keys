# customize below to fit your system

# paths
PREFIX = /usr/local

# includes
INCS = -I. -I/usr/include

# flags
CPPFLAGS = $(INCS)
# todo: revert to c++11 after removing the static struct initalisers
CFLAGS = -std=c++2a -pedantic -Wall -Wextra -O3
LDFLAGS =

# compiler and linker
CC = cc

