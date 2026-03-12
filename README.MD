# Neural Network from Scratch

A self-driving car trained using reinforcement learning (REINFORCE / policy gradient), built entirely from scratch — no ML frameworks, no physics engines, no game engines.

The car learns to drive around a track by receiving distance sensor readings and outputting steering and acceleration adjustments. The physics simulation and rendering are implemented in C; training is done in Python using only NumPy.

## Demo

The trained car navigates a track using 9 ray-cast sensors:

```
[ray distances × 9] + speed + acceleration + steering_angle
                          ↓
                   Neural Network (12→24→16→2)
                          ↓
              [Δacceleration, Δsteering_angle]
```

## Project Structure

```
Neural-Network-from-Scratch/
├── python/                   # Training code
│   ├── network.py            # Pure NumPy neural network (forward + backprop)
│   ├── train.py              # REINFORCE training loop
│   ├── simulator.py          # Python ctypes wrapper for the C simulator
│   ├── utils.py              # Discounted returns computation
│   ├── file_save.py          # Export trained weights to binary for C
│   ├── best_weights.npy      # Best saved weights (NumPy format)
│   └── weights.bin           # Exported weights (C-readable binary)
└── simulator/                # C physics engine + OpenGL visualizer
    ├── src/                  # C source files
    ├── include/              # C headers
    ├── renderer/             # OpenGL rendering (window, shaders, car/track/ray)
    ├── tracks/               # Track definition files (.txt)
    ├── track_drawer/         # Python tool for drawing custom tracks
    ├── Makefile              # Build system
    └── libsimulator.dylib    # Shared library used by Python training
```

## How It Works

### Training (Python)
1. The Python `Simulator` class loads `libsimulator.dylib` via ctypes
2. Each episode: the network samples actions from a Gaussian policy, runs a full episode, and collects the trajectory
3. Discounted returns are computed and normalized, then a REINFORCE gradient update is applied
4. Exploration noise (`sigma`) decays over time from 1.0 → 0.2
5. Best weights (by 10-episode average reward) are saved to `best_weights.npy`
6. `file_save.py` exports those weights to `weights.bin` for use by the C visualizer

### Visualization (C)
The standalone `simulator` binary loads `weights.bin`, runs the trained network in real time, and renders the car driving around the track using OpenGL.

### Reward
- Each step: reward = forward progress along the track
- Episode end (success): +1000 bonus
- Episode ends on collision or going off-track

## Neural Network

- **Architecture**: 3 fully-connected layers — `12 → 24 → 16 → 2`
- **Activations**: tanh on all layers
- **Inputs (12)**: 9 normalized ray distances + speed + acceleration + steering angle
- **Outputs (2)**: Δacceleration, Δsteering angle
- **Algorithm**: REINFORCE with Gaussian policy and entropy via sigma decay
- **Optimizer**: Vanilla SGD with gradient clipping (±1.0)

## Physics Constants

| Parameter | Value |
|---|---|
| Max forward speed | 5.0 m/s |
| Max acceleration | 1.0 m/s² |
| Max steering angle | ±30° |
| Ray count | 9 |
| Max ray distance | 10.0 m |
| Timestep (dt) | 0.01 s |

Ray angles (radians): `±75°, ±45°, ±25°, ±10°, 0°`

## Getting Started

### Prerequisites

- macOS (uses `.dylib` shared library format)
- Python 3.10+, NumPy
- GCC, GLFW, OpenGL (for the C simulator)

```bash
brew install glfw
```

### Build the C Simulator

```bash
cd simulator
make sim_lib      # Build libsimulator.dylib (used by Python training)
make simulator    # Build the OpenGL visualizer
```

### Train

```bash
cd python
python train.py
```

Prints episode rewards, average over last 10 episodes, step count, and sigma each 10 episodes.

### Export Weights and Visualize

```bash
cd python
python file_save.py      # Exports best_weights.npy → weights.bin

cd ../simulator
./simulator              # Runs the trained car in the OpenGL window
```

## Technologies

| Layer | Technology |
|---|---|
| Neural network | Pure NumPy (no PyTorch/TF) |
| Physics + collision | C (from scratch) |
| Spatial indexing | Quad-tree (C) |
| Rendering | OpenGL + GLFW (C) |
| Python/C bridge | ctypes |
| Track format | Custom .txt with Bézier-sampled boundaries |

## Acknowledgments

Inspired by [Green Code on YouTube](https://www.youtube.com/@GreenCode). Built to understand every layer of how a self-driving agent actually works, from physics integration to policy gradient math.
