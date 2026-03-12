import numpy as np
from network import NeuralNetwork

np.random.seed(0)

x = np.random.randn(12).astype(np.float64)
target = np.array([0.5, -0.3], dtype=np.float64)


def mse(a3, target):
    return np.mean((a3 - target) ** 2)


def gradient_check(nn, x, target, eps=1e-5, num_checks=5):
    # Forward + backward once to get analytical grads
    a3, cache = nn.forward(x)
    grads = nn.backward(cache, target)

    param_map = {
        "w1": nn.w1,
        "b1": nn.b1,
        "w2": nn.w2,
        "b2": nn.b2,
        "w3": nn.w3,
        "b3": nn.b3,
    }

    print("=== Gradient Check ===")

    for name, param in param_map.items():
        grad = grads[name]

        print(f"\nChecking {name} ...")

        # Sample random indices
        if param.ndim == 2:
            rows, cols = param.shape
            for _ in range(num_checks):
                i = np.random.randint(rows)
                j = np.random.randint(cols)

                original = param[i, j]

                # f(x + eps)
                param[i, j] = original + eps
                a3_plus, _ = nn.forward(x)
                loss_plus = mse(a3_plus, target)

                # f(x - eps)
                param[i, j] = original - eps
                a3_minus, _ = nn.forward(x)
                loss_minus = mse(a3_minus, target)

                # restore
                param[i, j] = original

                numerical = (loss_plus - loss_minus) / (2 * eps)
                analytical = grad[i, j]

                relative_error = abs(analytical - numerical) / (
                    abs(analytical) + abs(numerical) + 1e-12
                )

                print(
                    f"{name}[{i},{j}] | "
                    f"analytical={analytical:.8f} | "
                    f"numerical={numerical:.8f} | "
                    f"rel_error={relative_error:.8e}"
                )

        elif param.ndim == 1:
            size = param.shape[0]
            for _ in range(num_checks):
                i = np.random.randint(size)

                original = param[i]

                # f(x + eps)
                param[i] = original + eps
                a3_plus, _ = nn.forward(x)
                loss_plus = mse(a3_plus, target)

                # f(x - eps)
                param[i] = original - eps
                a3_minus, _ = nn.forward(x)
                loss_minus = mse(a3_minus, target)

                # restore
                param[i] = original

                numerical = (loss_plus - loss_minus) / (2 * eps)
                analytical = grad[i]

                relative_error = abs(analytical - numerical) / (
                    abs(analytical) + abs(numerical) + 1e-12
                )

                print(
                    f"{name}[{i}] | "
                    f"analytical={analytical:.8f} | "
                    f"numerical={numerical:.8f} | "
                    f"rel_error={relative_error:.8e}"
                )


nn = NeuralNetwork()
gradient_check(nn, x, target)