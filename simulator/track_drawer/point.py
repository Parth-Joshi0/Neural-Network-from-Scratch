from attr.validators import instance_of

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __sub__(self, other):
        if not isinstance(other, Point):
            raise TypeError("Subtraction only supported with other Class Points")
        new_x = self.x - other.x
        new_y = self.y - other.y
        return Point(new_x, new_y)

    def __add__(self, other):
        if not isinstance(other, Point):
            raise TypeError("Addition only supported with other Class Points")
        new_x = self.x + other.x
        new_y = self.y + other.y
        return Point(new_x, new_y)

    def __mul__(self, other):
        if not isinstance(other, (int, float)):
            raise TypeError("Multiplication only supported with int & Point")
        return Point(other*self.x, other*self.y)

    def __rmul__(self, other):
        return self.__mul__(other)

    def __repr__(self):
        return f"({self.x}, {self.y})"

    def __str__(self):
        return f"{self.x} {self.y}"

    def __truediv__(self, other):
        """Division by scalar"""
        if not isinstance(other, (int, float)):
            raise TypeError("Division only supported with numbers")
        return Point(self.x / other, self.y / other)

    def distance_to(self, other):
        """ Returns the Euclidean Distance between Points"""
        if not isinstance(other, Point):
            raise TypeError("Distance calculation only supported with other Points")
        dx = self.x - other.x
        dy = self.y - other.y
        return (dx**2 + dy**2) ** 0.5

    def normal(self):
        return Point(-1 * self.y, self.x)

    def magnitude(self):
        return (self.x**2 + self.y**2) ** 0.5

    def normalized(self):
        magnitude = self.magnitude()
        if magnitude == 0:
            return Point(0, 0)
        return self / magnitude