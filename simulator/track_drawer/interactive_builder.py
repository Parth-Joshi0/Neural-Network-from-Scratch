import matplotlib.pyplot as plt
import numpy as np
from bezier import CubicBezier
from file_io import save_track
from boundary import calculate_boundaries
from point import Point

def _pts_to_xy_np(points):
    return np.array([[p.x, p.y] for p in points], dtype=float)

class InteractiveTrackBuilder:
    def __init__(self, width=5.0, points_per_segment=100):
        self.width = width
        self.points_per_segment = points_per_segment

        self.segments: list[CubicBezier] = []
        self.current_control_points: list[Point] = []

        # Plot Setup
        self.fig, self.ax = plt.subplots(figsize=(12,8))
        self.ax.set_aspect('equal')
        self.ax.grid(True)
        self.ax.set_title('Track Builder - Click to place control points\n'
                         'First segment: 4 clicks, Next segments: 3 clicks\n'
                         'Press ENTER to finish, ESC to undo last point')

        self.ax.set_xlim(0, 100)
        self.ax.set_ylim(0, 100)
        self.ax.set_autoscale_on(False)

        # Setting up Connections
        self.cid_click = self.fig.canvas.mpl_connect('button_press_event', self.on_click)
        self.cid_key = self.fig.canvas.mpl_connect('key_press_event', self.on_key)

        # Visual elements
        self.control_point_markers = []
        self.curve_lines = []
        self.boundary_lines = []

        plt.show()

    def on_click(self, event):
        if event.inaxes != self.ax:
            return

        # Add control point
        p = Point(event.xdata, event.ydata)
        self.current_control_points.append(p)

        # Plot the point
        marker, = self.ax.plot(event.xdata, event.ydata, 'ro', markersize=8)
        self.control_point_markers.append(marker)

        # Determine how many points are needed
        if len(self.segments) == 0:
            points_needed = 4  # First segment needs 4 points
        else:
            points_needed = 3  # Subsequent segments reuse last end point

        # If we have enough points, create the segment
        if len(self.current_control_points) == points_needed:
            self.create_segment()
            self.fig.canvas.draw()

    def create_segment(self):
        if len(self.segments) == 0:
            w0, w1, w2, w3 = self.current_control_points # First Segment Use All four points
        else:
            w0 = self.segments[-1].w3 # Every other Segment use previous end point as start point
            w1, w2, w3 = self.current_control_points

        curve = CubicBezier(w0, w1, w2, w3)
        self.segments.append(curve)

        self.update_visualization()
        self.current_control_points = []

    def update_visualization(self):
        for line in self.curve_lines + self.boundary_lines:
            line.remove()
        self.curve_lines = []
        self.boundary_lines = []

        # Draw all segments
        all_centerline = []
        all_tangents = []

        for segment in self.segments:
            points, tangents = segment.sample_arc_length(self.points_per_segment)
            all_centerline.extend(points)
            all_tangents.extend(tangents)

            # Draw centerline
            points_array = _pts_to_xy_np(points)
            line, = self.ax.plot(points_array[:, 0], points_array[:, 1], 'b-', linewidth=2)
            self.curve_lines.append(line)

        # Calculate and draw boundaries
        if all_centerline:
            left_boundary, right_boundary = calculate_boundaries(
                all_centerline, all_tangents, self.width
            )

            left_array = _pts_to_xy_np(left_boundary)
            right_array = _pts_to_xy_np(right_boundary)

            left_line, = self.ax.plot(left_array[:, 0], left_array[:, 1], 'g--', linewidth=1.5, label='Left')
            right_line, = self.ax.plot(right_array[:, 0], right_array[:, 1], 'r--', linewidth=1.5, label='Right')

            self.boundary_lines.extend([left_line, right_line])
            self.ax.legend()

        self.fig.canvas.draw()

    def on_key(self, event):
        if event.key == 'enter':
            # Finish and save
            if len(self.segments) > 0:
                filename = input("Enter filename (e.g., track_001.txt): ")
                self.save_track(filename)
                print(f"Track saved to {filename}")
                plt.close()

        elif event.key == 'escape':
            # Undo last point
            if self.current_control_points:
                self.current_control_points.pop()
                if self.control_point_markers:
                    self.control_point_markers[-1].remove()
                    self.control_point_markers.pop()
                self.fig.canvas.draw()

    def save_track(self, filename):
        # Collect all points
        all_centerline = []
        all_tangents = []

        for segment in self.segments:
            points, tangents = segment.sample_arc_length(self.points_per_segment)
            all_centerline.extend(points)
            all_tangents.extend(tangents)

        # Calculate boundaries
        left_boundary, right_boundary = calculate_boundaries(
            all_centerline, all_tangents, self.width
        )

        # Save to file
        save_track(filename, self.segments, left_boundary, right_boundary, self.width)