CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c11
LDFLAGS = -lm

simulator: main.o track_loader.o
	$(CC) -o simulator main.o track_loader.o $(LDFLAGS)

main.o: src/main.c
	$(CC) -c src/main.c $(CFLAGS)

track_loader.o: src/track_loader.c
	$(CC) -c src/track_loader.c $(CFLAGS)

clean:
	rm -f *.o simulator
