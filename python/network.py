import numpy as np
from simulator import Simulator

class NeuralNetwork:
    def __init__(self):
        self.w1 = np.random.normal(0, np.sqrt(1 / 12), size=(24, 12)).astype(np.float32)
        self.w2 = np.random.normal(0, np.sqrt(1 / 24), size=(16, 24)).astype(np.float32)
        self.w3 = np.random.normal(0, np.sqrt(1 / 16), size=(2, 16)).astype(np.float32)

        self.b1 = np.zeros(24).astype(np.float32)
        self.b2 = np.zeros(16).astype(np.float32)
        self.b3 = np.zeros(2).astype(np.float32)

    def forward(self, x) -> tuple[np.ndarray, dict]:
        z1 = self.w1 @ x + self.b1
        a1 = np.tanh(z1)

        z2 = self.w2 @ a1 + self.b2
        a2 = np.tanh(z2)

        z3 = self.w3 @ a2 + self.b3
        a3 = np.tanh(z3)

        cache = {
            'a0': x,
            'z1': z1, 'a1': a1,
            'z2': z2, 'a2': a2,
            'z3': z3, 'a3': a3,
        }

        return a3, cache

    def backward(self, cache: dict, target):
        a0 = cache['a0']
        z1, a1 = cache['z1'], cache['a1']
        z2, a2 = cache['z2'], cache['a2']
        z3, a3 = cache['z3'], cache['a3']

        target = np.asarray(target, dtype=np.float32)

        # Number of output units
        n = target.shape[0]

        # Output layer
        dL_da3 = 2 * (a3 - target) / n
        dL_dz3 = dL_da3 * (1 - a3 ** 2)

        dL_dW3 = np.outer(dL_dz3, a2)
        dL_db3 = dL_dz3

        # Layer 2
        dL_da2 = self.w3.T @ dL_dz3
        dL_dz2 = dL_da2 * (1 - a2 ** 2)

        dL_dW2 = np.outer(dL_dz2, a1)
        dL_db2 = dL_dz2

        # Layer 1
        dL_da1 = self.w2.T @ dL_dz2
        dL_dz1 = dL_da1 * (1 - a1 ** 2)

        dL_dW1 = np.outer(dL_dz1, a0)
        dL_db1 = dL_dz1

        grads = {
            'w1': dL_dW1,
            'b1': dL_db1,
            'w2': dL_dW2,
            'b2': dL_db2,
            'w3': dL_dW3,
            'b3': dL_db3,
        }

        return grads