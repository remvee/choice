# choice: makefile
#	R.W.van 't Veer, Amsterdam, 16.IV.96
#
# $Id: makefile,v 1.3 1998-07-11 22:28:06 remco Exp $

prefix =	/usr/local
BINDIR =	$(prefix)/bin
MAN1DIR =	$(prefix)/man/man1

CC =		gcc
CFLAGS =	-Wall -O2
LDFLAGS =
LIBS =		-lncurses

FILES =		Makefile choice.1 choice.c choice.h \
		keymap.c keymap.h line.c line.h main.c
OBJS =		choice.o keymap.o line.o main.o

choice:	$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o choice $(LIBS)

install: choice
	install -s choice $(BINDIR)
	install -m444 choice.1 $(MAN1DIR)

clean:
	rm -f *.o choice
	rm -f a.out *~

backup:
	tar -chf choice.tar $(FILES)
	mv -b -V t choice.tar OLD

# dependencies

choice.o: choice.c keymap.h line.h choice.h
keymap.o: keymap.c keymap.h
line.o: line.c line.h
main.o: main.c keymap.h line.h choice.h
