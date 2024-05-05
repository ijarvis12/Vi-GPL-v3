VERSION=0.0.1

# GPL version 3 Vi Terminal Text Editor
# Copyright (c) 2023 by Ian P. Jarvis
# This program is licensed under the GPL version 3

DESTDIR=
BINDIR=/usr/local/bin
CC=gcc
LIBS=-lncurses
HDR=vi.h
SRC=vi.c vi_cmdmode.c vi_insmode.c vi_vismode.c
OBJ=vi.o vi_cmdmode.o vi_insmode.o vi_vismode.o
MISC=LICENSE Makefile README.md

all: a.out

a.out: ${OBJ}
        ${CC} ${OBJ} ${LIBS}

${OBJ}: ${HDR} ${SRC}
        ${CC} -c ${SRC}

install: a.out
        cp a.out ${DESTDIR}${BINDIR}/vi

uninstall:
        rm ${DESTDIR}${BINDIR}/vi

clean:
        rm ${OBJ} a.out
