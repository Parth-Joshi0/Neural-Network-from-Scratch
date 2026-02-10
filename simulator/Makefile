CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c11
LDFLAGS = -lm

simulator: main.o track_loader.o car.o physics.o
	$(CC) -o simulator main.o track_loader.o car.o physics.o $(LDFLAGS)

main.o: src/main.c
	$(CC) -c src/main.c $(CFLAGS)

track_loader.o: src/track_loader.c
	$(CC) -c src/track_loader.c $(CFLAGS)

car.o: src/car.c
	$(CC) -c src/car.c $(CFLAGS)

physics.o: src/physics.c
	$(CC) -c src/physics.c $(CFLAGS)

clean:
	rm -f *.o simulator
