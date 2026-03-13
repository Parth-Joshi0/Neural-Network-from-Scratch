# Simulator

The C physics engine and OpenGL visualizer for the neural network car project. Compiled as both a shared library (`libsimulator.dylib`) for Python training and a standalone binary for real-time visualization.

## Architecture

```
simulator/
├── src/
│   ├── main.c              # Standalone visualizer entry point
│   ├── sim_lib.c           # Shared library API (init/reset/step/close)
│   ├── physics.c           # Car dynamics (acceleration, steering, velocity)
│   ├── car.c               # Car state management
│   ├── track_loader.c      # Parse track .txt files
│   ├── track_collision.c   # Collision detection using quad-tree
│   ├── ray_cast.c          # Ray-segment intersection
│   ├── quad_tree.c         # Spatial index over track boundary segments
│   ├── nn.c                # Inference-only neural network (loads weights.bin)
│   └── util.c              # Math helpers (clamp, etc.)
├── renderer/
│   ├── src/
│   │   ├── window.c        # GLFW window setup
│   │   ├── shader.c        # OpenGL shader loading
│   │   ├── track_renderer.c
│   │   ├── car_renderer.c
│   │   └── ray_renderer.c
│   └── include/
├── include/                # All C headers
├── tracks/                 # Track definition files
├── track_drawer/           # Python tool for creating tracks
└── Makefile
```

## Build Targets

```bash
make sim_lib     # libsimulator.dylib — shared library for Python ctypes training
make simulator   # Standalone OpenGL visualizer (loads weights.bin)
make test_lib    # Headless test binary for the sim library
make clean       # Remove build artifacts
```

Dependencies: GCC, GLFW, OpenGL (macOS). Install GLFW with `brew install glfw`.

## Shared Library API (`sim_lib.h`)

Used by Python's `simulator.py` via ctypes:

```c
int  sim_init(const char* track_filename, float car_start_x, float car_start_y, float car_start_heading);
void sim_reset(float* state_out);       // state_out[12]
void sim_step(float delta_accel, float delta_steering,
              float* state_out, float* reward_out, int* alive_out, int* success_out);
void sim_get_state(float* state_out);
void sim_close(void);
```

## State Vector (12 floats)

| Index | Value | Normalization |
|---|---|---|
| 0–8 | Ray distances (9 rays) | ÷ MAX_RAY_DISTANCE (10.0) |
| 9 | Speed | ÷ MAX_FORWARD_SPEED (5.0) |
| 10 | Acceleration | ÷ MAX_ACCELERATION (1.0) |
| 11 | Steering angle | ÷ MAX_STEERING_ANGLE (0.524 rad / 30°) |

Ray angles (relative to car heading): `±75°, ±45°, ±25°, ±10°, 0°`

## Physics Model

- **Integration**: Euler, dt = 0.01 s
- **Steering**: Bicycle model — heading updated by `speed × tan(steering_angle) / wheelbase`
- **Speed**: Clamped to `[MAX_REVERSE_SPEED, MAX_FORWARD_SPEED]`
- **Acceleration**: Clamped to `[-MAX_ACCELERATION, MAX_ACCELERATION]`
- **Steering**: Clamped to `[-MAX_STEERING_ANGLE, MAX_STEERING_ANGLE]`

## Collision Detection

Track boundary segments are indexed in a **quad-tree** at load time. Each frame:
1. Ray casts query the quad-tree for nearest boundary intersection
2. Collision check tests if the car's bounding box overlaps any boundary segment

This keeps both operations O(log n) regardless of track length.

## Neural Network Inference (`nn.c`)

The C `Network` struct mirrors the Python architecture exactly:

```c
// Architecture: 12 → 24 → 16 → 2, all tanh
typedef struct {
    float w1[24][12]; float b1[24];
    float w2[16][24]; float b2[16];
    float w3[2][16];  float b3[2];
} Network;
```

Weights are loaded from `../python/weights.bin` — a raw float32 binary written by `python/file_save.py` in layer order (w1, b1, w2, b2, w3, b3).

## Track Format

Tracks are `.txt` files with Bézier control points and pre-sampled boundaries:

```
WIDTH 5.0
SEGMENTS 2

SEGMENT 0
CONTROL_POINTS 4
0.0 0.0
2.0 5.0
8.0 5.0
10.0 0.0
...

LEFT_BOUNDARY 200
0.0 2.5
...

RIGHT_BOUNDARY 200
0.0 -2.5
...
```

### Creating Tracks

Use the interactive Python tool in `track_drawer/`:
1. Click to place 4 control points for the first Bézier segment
2. Click 3 more for each subsequent segment (start is auto-connected)
3. Press `ESC` to undo, `ENTER` to save

The tool uses cubic Bézier curves with arc-length parameterization for uniform boundary point spacing.

## Episode Termination

An episode ends when:
- Car collides with a track boundary
- Car successfully crosses the finish line (detected by perpendicular projection past the last boundary point within a 5-unit lateral tolerance)

On success, the car is reset to the start. A +1000 reward bonus is added in the Python training loop.

## Development Status

- [x] Physics engine
- [x] Quad-tree spatial index
- [x] Ray casting
- [x] Collision detection
- [x] Track loader
- [x] OpenGL rendering (track, car, rays)
- [x] Shared library API for Python
- [x] C-side neural network inference
- [x] Bézier track drawer
- [ ] Multi-instance parallelization for batch training
