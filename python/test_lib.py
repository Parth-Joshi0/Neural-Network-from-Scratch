from simulator import Simulator

NUM_TEST_STEPS = 100

def print_state(state):
    print(f"  Rays:     {' '.join(f'{v:.3f}' for v in state[:9])}")
    print(f"  Speed:    {state[9]:.3f}  Accel: {state[10]:.3f}  Steering: {state[11]:.3f}")

# --- 1. Init ---
print("=== sim_init ===")
sim = Simulator("tracks/test2.txt", 8.0, 9.3, 0.0)
print("PASS: track loaded\n")

# --- 2. Reset ---
print("=== sim_reset ===")
state = sim.reset()
print("Initial state:")
print_state(state)
print()

# --- 3. Step forward with gentle acceleration, no steering ---
print(f"=== sim_step x{NUM_TEST_STEPS} (accel=0.1, steer=0.0) ===")
survived = 0
for i in range(NUM_TEST_STEPS):
    state, reward, alive, success = sim.step(0.1, 0.0)
    print(f"Step {i+1:2d} | reward={reward:.4f} alive={int(alive)} success={int(success)}")
    print_state(state)
    if not alive or success:
        print(f"  --> Episode ended at step {i+1} (alive={int(alive)} success={int(success)})")
        break
    survived += 1
print(f"Survived {survived} steps\n")

# --- 4. Test reset clears state properly ---
print("=== sim_reset (second time) ===")
state = sim.reset()
print("State after second reset:")
print_state(state)
print()

# --- 5. Test steering response ---
print("=== sim_step x5 (accel=0.2, steer=0.1) ===")
for i in range(5):
    state, reward, alive, success = sim.step(0.2, 0.1)
    print(f"Step {i+1:2d} | reward={reward:.4f} alive={int(alive)} success={int(success)}")
    print_state(state)
    if not alive or success:
        break
print()

# --- 6. Close ---
print("=== sim_close ===")
sim.close()
print("PASS: closed cleanly")
