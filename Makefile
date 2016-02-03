CC = g++
DEBUG = -g -O0 -pedantic-errors
CFLAGS = -Wall -std=c++11 -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all: editor

editor: editor.o pfunc.o
	$(CC) $(LFLAGS) -o editor editor.o pfunc.o -lncurses -lmenu

editor.o: editor.cpp pfunc.h
	$(CC) $(CFLAGS) editor.cpp pfunc.h -lncurses -lnmenu

pfunc.o: pfunc.cpp pfunc.h
	$(CC) $(CFLAGS) pfunc.cpp

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf editor	
