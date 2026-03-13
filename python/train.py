from simulator import Simulator
from network import NeuralNetwork
from utils import compute_returns
import numpy as np

lr = 3e-4
gamma = 0.99
num_episodes = 30000
sigma = 1.0
sigma_min = 0.27
decay_rate = 0.9995

np.random.seed(16)

sim = Simulator("tracks/track_001.txt", x=12.5, y=16.1, heading=0.0)
nn = NeuralNetwork()

best_avg = -np.inf
best_weights = None
recent_rewards = []

for episode in range(num_episodes):
    state = sim.reset()
    trajectory = []

    while True:
        action, logp, cache = nn.sample_action(state)
        next_state, reward, alive, success = sim.step(action[0], action[1])
        trajectory.append((logp, reward, action, cache))
        state = next_state
        if not alive or success:
            break

    rewards = [r for _, r, _, _ in trajectory]
    if success:
        rewards.append(1000)
    returns = compute_returns(rewards, gamma)

    mean, std = returns.mean(), returns.std()
    if std > 1e-8:
        returns = (returns - mean) / std

    total_grads = None
    for t, (logp, reward, action, cache) in enumerate(trajectory):
        grads = nn.backward(cache, G=returns[t], action=action, mode='policy')
        if total_grads is None:
            total_grads = grads
        else:
            for key in grads:
                total_grads[key] += grads[key]

    for key in total_grads:
        total_grads[key] = np.clip(total_grads[key], -1.0, 1.0)

    nn.update(total_grads, lr)

    total_reward = sum(rewards)
    recent_rewards.append(total_reward)
    if len(recent_rewards) > 10:
        recent_rewards.pop(0)

    avg10 = np.mean(recent_rewards)

    if avg10 > best_avg:
        best_avg = avg10
        best_weights = {
            'w1': nn.w1.copy(), 'b1': nn.b1.copy(),
            'w2': nn.w2.copy(), 'b2': nn.b2.copy(),
            'w3': nn.w3.copy(), 'b3': nn.b3.copy(),
        }
        np.save('best_weights.npy', nn.get_weights())

    sigma = max(sigma_min, sigma * decay_rate)
    nn.sigma = sigma

    if (episode + 1) % 10 == 0 or success:
        print(f"Episode {episode + 1:4d} | reward: {total_reward:8.2f} | avg10: {avg10:8.2f} | steps: {len(trajectory):4d} | sigma: {sigma:.3f} | success: {success}")

sim.close()
