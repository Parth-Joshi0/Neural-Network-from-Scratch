import ctypes
import numpy as np
from pathlib import Path

class Simulator:
    def __init__(self, track: str, x, y, heading):
        sim_path = Path(__file__).parent / ".." / "simulator"

        lib_path = sim_path / "libsimulator.dylib"
        self.lib = ctypes.CDLL(str(lib_path))

        self.lib.sim_init.argtypes = [
            ctypes.c_char_p,
            ctypes.c_float,
            ctypes.c_float,
            ctypes.c_float,
        ]
        self.lib.sim_init.restype = ctypes.c_int

        self.lib.sim_reset.argtypes = [
            ctypes.POINTER(ctypes.c_float)
        ]
        self.lib.sim_reset.restype = None

        self.lib.sim_step.argtypes = [
            ctypes.c_float,
            ctypes.c_float,
            ctypes.POINTER(ctypes.c_float),
            ctypes.POINTER(ctypes.c_float),
            ctypes.POINTER(ctypes.c_int),
            ctypes.POINTER(ctypes.c_int),
        ]
        self.lib.sim_step.restype = None

        self.lib.sim_get_state.argtypes = [
            ctypes.POINTER(ctypes.c_float)
        ]
        self.lib.sim_get_state.restype = None

        self.lib.sim_close.argtypes = []
        self.lib.sim_close.restype = None

        output_array = ctypes.c_float * 12
        self.state_out = output_array()

        self.reward = ctypes.c_float()
        self.alive = ctypes.c_int()
        self.success = ctypes.c_int()

        track_path = sim_path / track
        ok = self.lib.sim_init(str(track_path).encode("utf-8"), x, y, heading)

        if ok == 1:
            raise ValueError("Failed simulator initialization")

    def reset(self) -> np.ndarray:
        self.lib.sim_reset(self.state_out)
        return np.ctypeslib.as_array(self.state_out, shape=(12,))

    def step(self, delta_accel, delta_steer) -> tuple[np.ndarray, float, bool, bool]:
        self.lib.sim_step(delta_accel, delta_steer, self.state_out, ctypes.byref(self.reward), ctypes.byref(self.alive), ctypes.byref(self.success))
        state = np.ctypeslib.as_array(self.state_out, shape=(12,))
        reward = float(self.reward.value)
        alive = bool(self.alive.value)
        success = bool(self.success.value)
        return state, reward, alive, success

    def close(self):
        self.lib.sim_close()