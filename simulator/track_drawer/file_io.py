from simulator.track_drawer.point import Point

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