# Makefile Example... by FUNFUN.YOO

CC = gcc
STRIP = strip

SRC1 = main
SRC2 = libfunc1
SRC3 = libfunc2

all: funfun

funfun: $(SRC1).o $(SRC2).o $(SRC3).o
	$(CC) -o funfun $(SRC1).o $(SRC2).o $(SRC3).o
	$(STRIP) funfun

$(SRC1).o: $(SRC1).c
	$(CC) -c $(SRC1).c

$(SRC2).o: $(SRC2).c
	$(CC) -c $(SRC2).c

$(SRC3).o: $(SRC3).c
	$(CC) -c $(SRC3).c

clean:
	rm -f *.o funfun
 
