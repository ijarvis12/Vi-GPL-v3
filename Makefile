VERSION=0.0.1
/* GPL version 3 Vi Terminal Text Editor
 * Copyright 2023 by Ian P. Jarvis
 * This program is licensed under the GPL version 3
 */
DESTDIR=
BINDIR=/usr/local/bin
CC=gcc
CFLAGS=-Wall -Wextra -Werror I/usr/include -DTABS=4
LIBS=-lncurses
OBJS=vi.o vi_cmdmode.o vi_insmode.o vi_re.o vi_vismode.o
HDRS=vi.h vi_cmdmode.h vi_insmode.h vi_re.h vi_vismode.h
SRC=vi.c vi_cmdmode.c vi_insmode.c vi_re.c vi_vismode.c
MISC=LICENSE Makefile README.md

all: a.out

a.out: ${OBJS}
  ${CC} ${OBJS} ${LIBS}

vi.o: ${HDRS} ${SRC}
  ${CC} ${CFLAGS} -c vi.h vi.c

vi_cmdmode.o: ${HDRS} ${SRC}
  ${CC} ${CFLAGS} -c vi.h vi_re.h vi_cmdmode.h vi_cmdmode.c

vi_insmode.o: ${HDRS} ${SRC}
  ${CC} ${CFLAGS} -c vi.h vi_insmode.h vi_insmode.c

vi_re.o: ${HDRS} ${SRC}
  ${CC} ${CFLAGS} -c vi.h vi_re.h vi_re.c

vi_vismode.o: ${HDRS} ${SRC}
  ${CC} ${CFLAGS} -c vi.h vi_vismode.h vi_vismode.c

install: a.out
  cp a.out ${DESTDIR}${BINDIR}/vi

uninstall:
  rm ${DESTDIR}${BINDIR}/vi

clean:
  rm *.o a.out

lint: ${HDRS} ${SRC}
  splint ${HDRS} ${SRC}
