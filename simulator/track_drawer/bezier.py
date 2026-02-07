import numpy as np
from simulator.track_drawer.point import Point

class CubicBezier:
    def __init__(self, w0 : Point, w1: Point, w2: Point, w3: Point):
        """ The controlling weights of the bezier curve, with w0 being the Start, and w3 being the end"""
        self.w0 = w0
        self.w1 = w1
        self.w2 = w2
        self.w3 = w3

    def get_curve_point(self, points, t):
        """ Evaluate the curve at ratio t using de Casteljau's algorithm"""
        if len(points) == 1:
            return points[0]

        new_points = []
        for i in range(len(points) - 1):
            new_points.append((1-t) * points[i] + t * points[i+1])

        return self.get_curve_point(new_points, t)

    def evaluate(self, t):
        """ Evaluate the curve at parameter t"""
        points = [self.w0, self.w1, self.w2, self.w3]
        return self.get_curve_point(points, t)

    def derivative_weights(self):
        """ Return The Control Points for the Derivative (Quadratic)"""
        w0 = 3 * (self.w1 - self.w0)
        w1 = 3 * (self.w2 - self.w1)
        w2 = 3 * (self.w3 - self.w2)
        return (w0, w1, w2)

    def derivative_at(self, t):
        """ Evaluate the Derivative (tangent vector) at the ratio t"""
        w0, w1, w2 = self.derivative_weights()
        points = [w0, w1, w2]
        return self.get_curve_point(points, t)

    def sample_arc_length(self, num_points):
        """ Returns Sample Curve with approximately uniform arc-length spacing"""

        # Calculate the Dense Parametric Sampling of the Curve
        dense_sample = 1000
        dense_t = np.linspace(0,1, dense_sample)
        dense_points = [self.evaluate(t) for t in dense_t]

        # Calculate the cumulative distance
        distances = [0.0]
        for i in range(1, len(dense_points)):
            segment_length = dense_points[i].distance_to(dense_points[i-1])
            distances.append(distances[-1] + segment_length)

        total_length = distances[-1]

        # Determine Target Distance
        target_distances = np.linspace(0, total_length, num_points)

        # Find Corresponding Values
        t_values = np.interp(target_distances, distances, dense_t)

        # Resample at Found Values
        points = [self.evaluate(t) for t in t_values]
        tangent = [self.derivative_at(t) for t in t_values]

        return points, tangent

