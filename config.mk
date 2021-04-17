VERSION ?= "1.4.1-SNAPSHOT"

PREFIX ?= /usr/local

INCS += -I/usr/include/libevdev-1.0

CPPFLAGS += $(INCS) -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"

COMPFLAGS += -pedantic -Wall -Wextra -O3
CFLAGS += $(COMPFLAGS) -std=c99
CXXFLAGS += $(COMPFLAGS) -std=c++11

LDFLAGS += -rdynamic -lyaml-cpp -levdev

CC = cc
CXX = c++

