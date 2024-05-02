VERSION=0.0.1
/* GPL version 3 Vi Terminal Text Editor
 * Copyright (c) 2023 by Ian P. Jarvis
 * This program is licensed under the GPL version 3
 */
DESTDIR=
BINDIR=/usr/local/bin
CC=gcc
CFLAGS=-Wall -Wextra -Werror I/usr/include/
LIBS=-lncurses
OBJ=vi.o
HDRS=vi.h
SRC=vi.c vi_cmdmode.c vi_insmode.c vi_vismode.c
MISC=LICENSE Makefile README.md

all: a.out

a.out: ${OBJ}
\t${CC} ${OBJ} ${LIBS}

${OBJ}: ${HDRS} ${SRC}
\t${CC} ${CFLAGS} -c ${HDRS} ${SRC}

install: a.out
\tcp a.out ${DESTDIR}${BINDIR}/vi

uninstall:
\trm ${DESTDIR}${BINDIR}/vi

clean:
\trm ${OBJ} a.out
