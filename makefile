CFLAGS = -Wall
objects = main.o

all: craneflak

craneflak: $(objects)
	$(CC) -o craneflak $(objects) $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

debug: CFLAGS += -g -O0
debug: craneflak

clean:
	rm -f craneflak $(objects)
