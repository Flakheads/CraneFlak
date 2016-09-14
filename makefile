CFLAGS = -Wall
objects = main.o stack.o

all: craneflak

craneflak: $(objects)
	$(CC) -o craneflak $(objects) $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

stack.o: stack.h stack.c
	$(CC) -c stack.c $(CFLAGS)

debug: CFLAGS += -g -O0
debug: craneflak

clean:
	rm -f craneflak $(objects)
