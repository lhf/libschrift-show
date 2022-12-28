# show text using libschrift

# default args; paths for macOS
F= /System/Library/Fonts/NewYork.ttf
F= /System/Library/Fonts/NewYorkItalic.ttf
F= /System/Library/Fonts/Supplemental/Times New Roman Italic.ttf
S= 1000
C= abfgj.

CFLAGS = -O2 -std=c99 -pedantic -Wall -Wextra # -Wconversion

all:	run

run:	show
	./show '$F' $S '$C' > out.pgm
	open out.pgm

show:	show.c schrift.o
	$(CC) $(CFLAGS) -o $@ $@.c schrift.o -lm

schrift.o: schrift.c schrift.h

clean:
	rm -f show schrift.o out.pgm

.PHONY: all run clean

