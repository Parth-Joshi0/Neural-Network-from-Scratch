from simulator.track_drawer.point import Point
from simulator.track_drawer.bezier import CubicBezier

def save_track(track_name, segments, left_boundary, right_boundary, width):
    """ Save the Track to a file"""
    output_path = "../tracks/"
    with open(output_path + track_name, 'w') as f:
        f.write(f"WIDTH {width}\n")
        f.write(f"SEGMENTS {len(segments)}\n")
        f.write("\n")

        for i, segment in enumerate(segments):
            f.write(f"SEGMENT {i}\n")
            f.write(f"CONTROL_POINTS {4}\n")
            f.write(f"{segment.w0}\n")
            f.write(f"{segment.w1}\n")
            f.write(f"{segment.w2}\n")
            f.write(f"{segment.w3}\n")
            f.write(f"\n")

        f.write(f"LEFT_BOUNDARY {len(left_boundary)}\n")
        for point in left_boundary:
            f.write(f"{point}\n")
        f.write("\n")

        f.write(f"RIGHT_BOUNDARY {len(right_boundary)}\n")
        for point in right_boundary:
            f.write(f"{point}\n")

def load_track(filename):
    """ Takes out the data from the filename"""
    width = None
    segments = []
    left_boundary = []
    right_boundary = []

    def _next_nonempty(lines, idx):
        n = len(lines)
        while idx < n:
            s = lines[idx].strip()
            idx += 1
            if s != "":
                return s, idx
        return None, idx

    output_path = "../tracks/"
    with open(output_path + filename, "r") as f:
        lines = f.readlines()

    i = 0

    # Width
    line, i = _next_nonempty(lines, i)
    if not line or not line.startswith("WIDTH"):
        raise ValueError("Expected 'WIDTH <float>'")
    width = float(line.split()[1])

    # Segments
    line, i = _next_nonempty(lines, i)
    if not line or not line.startswith("SEGMENTS"):
        raise ValueError("Expected 'SEGMENTS <int>'")
    num_segments = int(line.split()[1])

    # SEGMENT blocks
    for _ in range(num_segments):
        line, i = _next_nonempty(lines, i)
        if not line or not line.startswith("SEGMENT"):
            raise ValueError("Expected 'SEGMENT <i>'")

        line, i = _next_nonempty(lines, i)
        if not line or not line.startswith("CONTROL_POINTS"):
            raise ValueError("Expected 'CONTROL_POINTS 4'")
        cp_count = int(line.split()[1])
        if cp_count != 4:
            raise ValueError(f"Expected 4 control points, got {cp_count}")

        cps = []
        for _ in range(4):
            line, i = _next_nonempty(lines, i)
            x_str, y_str = line.split()[:2]
            cps.append(Point(float(x_str), float(y_str)))

        w0, w1, w2, w3 = cps
        segments.append(CubicBezier(w0, w1, w2, w3))

    # LEFT_BOUNDARY
    line, i = _next_nonempty(lines, i)
    if not line or not line.startswith("LEFT_BOUNDARY"):
        raise ValueError("Expected 'LEFT_BOUNDARY <N>'")
    left_n = int(line.split()[1])

    for _ in range(left_n):
        line, i = _next_nonempty(lines, i)
        x_str, y_str = line.split()[:2]
        left_boundary.append(Point(float(x_str), float(y_str)))

    # RIGHT_BOUNDARY
    line, i = _next_nonempty(lines, i)
    if not line or not line.startswith("RIGHT_BOUNDARY"):
        raise ValueError("Expected 'RIGHT_BOUNDARY <N>'")
    right_n = int(line.split()[1])

    for _ in range(right_n):
        line, i = _next_nonempty(lines, i)
        x_str, y_str = line.split()[:2]
        right_boundary.append(Point(float(x_str), float(y_str)))

    return width, segments, left_boundary, right_boundary
