CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c11
LDFLAGS = -lm

simulator: main.o track_loader.o car.o physics.o quad_tree.o ray_cast.o util.o
	$(CC) -o simulator main.o track_loader.o car.o physics.o quad_tree.o ray_cast.o util.o $(LDFLAGS)

main.o: src/main.c
	$(CC) -c src/main.c $(CFLAGS)

test: test.o track_loader.o car.o physics.o quad_tree.o ray_cast.o util.o
	$(CC) -o test test.o track_loader.o car.o physics.o quad_tree.o ray_cast.o util.o $(LDFLAGS)

test.o: src/test.c include/util.h include/track_internals.h include/car.h include/physics.h include/ray_cast.h include/quad_tree.h
	$(CC) -c src/test.c $(CFLAGS)

track_loader.o: src/track_loader.c include/track_loader.h include/track_internals.h include/types.h include/util.h
	$(CC) -c src/track_loader.c $(CFLAGS)

car.o: src/car.c include/car.h include/car_internals.h include/types.h include/util.h
	$(CC) -c src/car.c $(CFLAGS)

physics.o: src/physics.c include/physics.h include/physics_constants.h include/car_internals.h include/types.h
	$(CC) -c src/physics.c $(CFLAGS)

quad_tree.o: src/quad_tree.c include/track_internals.h include/quad_tree.h include/types.h include/util.h
	$(CC) -c src/quad_tree.c $(CFLAGS)

ray_cast.o: src/ray_cast.c include/quad_tree.h include/ray_cast.h include/types.h
	$(CC) -c src/ray_cast.c $(CFLAGS)

util.o: src/util.c include/util.h
	$(CC) -c src/util.c $(CFLAGS)

clean:
	rm -f *.o simulator
