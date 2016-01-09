# choice: makefile
#	R.W.van 't Veer, Amsterdam, 16.IV.96

PREFIX =	/usr/local
BINDIR =	$(PREFIX)/bin
MAN1DIR =	$(PREFIX)/share/man/man1

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
	rm -f a.out *~ core tags

backup:
	tar -chf choice.tar $(FILES)
	mv -b -V t choice.tar OLD

# dependencies

choice.o: choice.c keymap.h line.h choice.h
keymap.o: keymap.c keymap.h
line.o: line.c line.h
main.o: main.c keymap.h line.h choice.h
