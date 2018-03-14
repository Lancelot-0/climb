# CLIMB Makefile

CC=gcc
LDFLAGS=-lncurses
CFLAGS=-std=c99 -Wall -Wundef -Wshadow -Wformat=2
SRC_DIR=.
EXE_DIR=.

CLIMB_SRCS = climb.c
CLIMB_OBJS = $(CLIMB_SRCS:.c=.o)

all: build clean

build: climb 

climb: $(CLIMB_OBJS)
	$(CC) -o $@ $(CLIMB_OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@rm -f $(CLIMB_OBJS)