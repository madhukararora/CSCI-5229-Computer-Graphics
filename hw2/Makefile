all: hw2
TARGET=hw2
CFLAGS=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
CLEAN= rm -rf *.o $(TARGET)

#compilation and linking
hw2: main.c
	gcc $(CFLAGS) -o $@ $^ $(LIBS)
#clean
clean:
	$(CLEAN)
