VERSION=0.0.2

# GPL version 3 Vi Terminal Text Editor
# Copyright (c) 2023 by Ian P. Jarvis
# This program is licensed under the GPL version 3

DESTDIR=
BINDIR=/usr/local/bin
CC=gcc
LIBS=-lncurses
HDR=vi.h
SRC=vi.c vi_cmdmode.c vi_insmode.c vi_vismode.c
MISC=LICENSE Makefile README.md

all: vi

vi: ${HDR} ${SRC}
        sed -i "s/#gain/#include/g" ${HDR} ${SRC}
        ${CC} -o vi ${HDR} ${SRC} -lncurses

install: vi
        cp vi ${DESTDIR}${BINDIR}/

uninstall:
        rm ${DESTDIR}${BINDIR}/vi

clean:
        rm ${OBJ} a.out
