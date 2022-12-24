# show text using libschrift

CFLAGS = -O2 -std=c99 -pedantic -Wall -Wextra -Wconversion 

all:	run

show:	show.c schrift.o
	$(CC) $(CFLAGS) -o $@ $@.c schrift.o -lm

schrift.o: schrift.c schrift.h

run:	show
	./show $F $S '$C' > out.pgm
	open out.pgm

F= /System/Library/Fonts/NewYork.ttf
S= 1000
C= abfgj

clean:
	rm -f show schrift.o out.pgm

.PHONY: all clean

