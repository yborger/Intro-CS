# makefile for pthread gol lab

CC = gcc
C++ = g++
CFLAGS = -g -Wall -Wvla -Werror -Wno-error=unused-variable

LIBS = -lpthread

all: gol

gol: gol.o
	$(C++) $(CFLAGS) -o gol gol.o  $(LIBS)

gol.o: gol.c
	$(CC) $(CFLAGS) -c gol.c

clean:
	$(RM) gol *.o
