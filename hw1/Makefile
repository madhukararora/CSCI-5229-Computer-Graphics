all: hw1

CFLAGS=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
CLEAN= rm -rf *.o hw1

#compilation and linking
hw1: main.c
	gcc $(CFLAGS) -o $@ $^ $(LIBS)
#clean
clean:
	$(CLEAN)
