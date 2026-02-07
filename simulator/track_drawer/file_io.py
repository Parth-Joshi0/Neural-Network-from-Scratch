from simulator.track_drawer.point import Point

def save_track(track_name, segments, left_boundary, right_boundary, width):
    """ Save the Track to a file"""
    output_path = "../tracks/"
    with open(output_path + track_name, 'w') as f:
        f.write(f"WIDTH {width}\n")
        f.write(f"SEGMENTS {len(segments)}\n")
        f.write("\n")

        for i, segments in enumerate(segments):
            f.write(f"SEGMENT {i}\n")
            f.write(f"CONTROL_POINTS {4}\n")
            f.write(f"{segments[0]}\n")
            f.write(f"{segments[1]}\n")
            f.write(f"{segments[2]}\n")
            f.write(f"{segments[3]}\n")
            f.write(f"\n")

        f.write(f"LEFT_BOUNDARY {len(left_boundary)}\n")
        for point in left_boundary:
            f.write(f"{point}\n")
        f.write("\n")

        f.write(f"RIGHT_BOUNDARY {len(right_boundary)}\n")
        for point in right_boundary:
            f.write(f"{point}\n")