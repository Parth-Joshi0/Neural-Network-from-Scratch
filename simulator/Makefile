CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c11 -I/opt/homebrew/include -Irenderer/include
LDFLAGS = -lm -L/opt/homebrew/lib -lglfw -framework OpenGL
COMMON_OBJS = track_loader.o car.o physics.o quad_tree.o ray_cast.o util.o track_collision.o window.o glad.o shader.o track_renderer.o car_renderer.o ray_renderer.o
SIM_LIB_OBJS = sim_lib.o track_loader.o car.o physics.o quad_tree.o ray_cast.o util.o track_collision.o

sim_lib: $(SIM_LIB_OBJS)
	$(CC) -dynamiclib -o libsimulator.dylib $(SIM_LIB_OBJS) -lm

test_lib: test_lib.o $(SIM_LIB_OBJS)
	$(CC) -o test_lib test_lib.o $(SIM_LIB_OBJS) -lm

simulator: main.o $(COMMON_OBJS)
	$(CC) -o simulator main.o $(COMMON_OBJS) $(LDFLAGS)

test: test.o $(COMMON_OBJS)
	$(CC) -o test test.o $(COMMON_OBJS) $(LDFLAGS)

main.o: src/main.c
	$(CC) -c src/main.c $(CFLAGS)

test.o: src/test.c $(COMMON_OBJS)
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

track_collision.o: src/track_collision.c include/track_collision.h include/types.h include/car.h include/car_internals.h include/quad_tree.h
	$(CC) -c src/track_collision.c $(CFLAGS)
	
window.o: renderer/src/window.c renderer/include/window.h
	$(CC) -c renderer/src/window.c $(CFLAGS)

glad.o: src/glad.c include/glad.h
	$(CC) -c src/glad.c $(CFLAGS)

shader.o: renderer/src/shader.c renderer/include/shader.h include/glad.h
	$(CC) -c renderer/src/shader.c $(CFLAGS)

track_renderer.o: renderer/src/track_renderer.c renderer/include/track_renderer.h renderer/include/shader.h include/glad.h include/track_internals.h
	$(CC) -c renderer/src/track_renderer.c $(CFLAGS)

car_renderer.o: renderer/src/car_renderer.c renderer/include/car_renderer.h renderer/include/shader.h include/glad.h include/car_internals.h
	$(CC) -c renderer/src/car_renderer.c $(CFLAGS)

ray_renderer.o: renderer/src/ray_renderer.c renderer/include/ray_renderer.h renderer/include/shader.h include/glad.h include/car_internals.h
	$(CC) -c renderer/src/ray_renderer.c $(CFLAGS)

sim_lib.o: src/sim_lib.c include/sim_lib.h
	$(CC) -c src/sim_lib.c $(CFLAGS) -fPIC

test_lib.o: src/test_lib.c
	$(CC) -c src/test_lib.c $(CFLAGS)

clean:
	rm -f *.o simulator test
