CFLAGS = -Wall
objects = main.o stack.o interpreter.o

all: craneflak

craneflak: $(objects)
	$(CC) -o craneflak $(objects) $(CFLAGS)

main.o: main.c stack.h interpreter.h
	$(CC) -c main.c $(CFLAGS)

stack.o: stack.h stack.c
	$(CC) -c stack.c $(CFLAGS)

interpreter.o: interpreter.h interpreter.c stack.h
	$(CC) -c interpreter.c $(CFLAGS)

debug: CFLAGS += -g -O0
debug: craneflak

clean:
	rm -f craneflak $(objects)
