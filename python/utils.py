import numpy as np

def compute_returns(rewards: list, gamma=0.99) -> np.ndarray:
    returns = np.zeros(len(rewards))
    running_return = 0.0

    for t in reversed(range(len(rewards))):
        running_return = rewards[t] + gamma * running_return
        returns[t] = running_return

    return returns