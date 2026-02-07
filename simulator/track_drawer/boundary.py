from simulator.track_drawer.point import Point

def calculate_boundaries(centerline_points, tangent_vectors, width):
    """ Given the center points and tangent vectors, calculates the left and right boundaries of the track"""
    left_boundary = []
    right_boundary = []

    half_width = width * 0.5

    for point, tangent in zip(centerline_points, tangent_vectors):
        tangent_normalized = tangent.normalized()

        normal = tangent_normalized.normal()

        left_point = tangent_normalized + normal * half_width
        right_point = tangent_normalized - normal * half_width

        left_boundary.append(left_point)
        right_boundary.append(right_point)

    return left_boundary, right_boundary

