import numpy as np

weights = np.load('best_weights.npy', allow_pickle=True).item()

with open('weights.bin', 'wb') as f:
    weights['w1'].astype(np.float32).tofile(f)
    weights['b1'].astype(np.float32).tofile(f)
    weights['w2'].astype(np.float32).tofile(f)
    weights['b2'].astype(np.float32).tofile(f)
    weights['w3'].astype(np.float32).tofile(f)
    weights['b3'].astype(np.float32).tofile(f)