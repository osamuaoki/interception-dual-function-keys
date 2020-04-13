# customize below to fit your system

# includes
INCS = -I. -I/usr/include

# flags
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O3 ${INCS}
LDFLAGS =

# compiler and linker
CC = cc
