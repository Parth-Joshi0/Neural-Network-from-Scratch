# Python Training

REINFORCE (policy gradient) training for the self-driving car. Implemented from scratch using only NumPy — no PyTorch, no TensorFlow.

## Files

| File | Purpose |
|---|---|
| `train.py` | Training loop — runs episodes, computes returns, updates the network |
| `network.py` | Neural network — forward pass, backprop, policy sampling |
| `simulator.py` | ctypes wrapper around `libsimulator.dylib` |
| `utils.py` | `compute_returns` — discounted reward-to-go |
| `file_save.py` | Export `best_weights.npy` → `weights.bin` for the C visualizer |
| `best_weights.npy` | Best saved weights (NumPy format) |
| `weights.bin` | Raw float32 binary weights loaded by the C simulator |

## Usage

### 1. Build the shared library first

```bash
cd ../simulator
make sim_lib
```

### 2. Train

```bash
python train.py
```

Output per 10 episodes:
```
Episode  10 | reward:  -12.34 | avg10:  -15.21 | steps:   47 | sigma: 0.999 | success: False
```

### 3. Export weights for the C visualizer

```bash
python file_save.py
```

This reads `best_weights.npy` and writes `weights.bin` in raw float32 order (w1, b1, w2, b2, w3, b3).

Then run the visualizer:
```bash
cd ../simulator
./simulator
```

## Neural Network (`network.py`)

**Architecture**: `12 → 24 → 16 → 2`, tanh activations on all layers

```
Input (12)   →  Hidden 1 (24)  →  Hidden 2 (16)  →  Output (2)
ray ×9            tanh               tanh              tanh
speed
acceleration
steering_angle
```

**Weights initialized** with He-style scaling: `N(0, sqrt(1/fan_in))`

### Forward pass
Standard matrix multiply + bias + tanh at each layer. Returns activations and a cache dict for backprop.

### Backward pass (`mode='policy'`)
Policy gradient loss. Given return `G` and sampled action `a`, the gradient of the log-probability loss is:

```
dL/da3 = -G * (action - mu) / sigma²
```

Backpropagated through tanh layers using the identity `d/dz tanh(z) = 1 - tanh²(z)`.

Also supports `mode='mse'` for supervised pretraining.

### Action sampling
Actions are sampled from a Gaussian policy:

```
action ~ N(mu, sigma²)
mu = network_output(state)
```

`sigma` controls exploration and decays during training.

## Training Loop (`train.py`)

| Hyperparameter | Value |
|---|---|
| Learning rate | 0.0003 |
| Discount factor γ | 0.99 |
| Episodes | 350,000 |
| Initial sigma | 1.0 |
| Minimum sigma | 0.2 |
| Sigma decay rate | 0.99999 per episode |
| Success bonus | +1000 |
| Gradient clip | ±1.0 |

**Per episode:**
1. Reset simulator, collect full trajectory `(logp, reward, action, cache)`
2. Compute discounted returns, normalize to zero mean / unit std
3. Accumulate REINFORCE gradients across all timesteps
4. Clip gradients to ±1.0, apply SGD update
5. Track 10-episode rolling average; save weights if it improves

## Simulator Interface (`simulator.py`)

Thin ctypes wrapper around `libsimulator.dylib`. Exposes three methods:

```python
sim = Simulator("tracks/track_001.txt", x=12.5, y=16.1, heading=0.0)

state = sim.reset()                          # np.ndarray shape (12,)
state, reward, alive, success = sim.step(delta_accel, delta_steer)
sim.close()
```

The state vector is the same 12-float normalized vector used by the C visualizer (9 ray distances + speed + acceleration + steering angle).

## Dependencies

```
numpy
```

The simulator itself is a compiled C shared library — no other Python dependencies.
