NCURSESINC = /usr/include
NCURSESLIB = /usr/lib


INCS = -I${NCURSESINC}
LIBS = -L${NCURSESLIB} -lncurses

CFLAGS = -std=c99 -pedantic -Wall -Os $(INCS)
LDFLAGS = -s $(LIBS)

CC = gcc
