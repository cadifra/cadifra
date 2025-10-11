/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Point;

export import d1.types;

import d1.round;
import d1.isEqual;

import std;


namespace d1
{

// forward declarations

struct Point;
struct fPoint;
struct Vector;
struct fVector;


// struct definitions

export struct Point
{
    using value_type = int32;

    value_type x{}, y{};

    bool operator==(const Point&) const = default;

    Point& Move(const Vector& offset);

    bool IsNear(const Point& rhs, int32 distance) const;

    Point& operator+=(const Vector& v);
};


export struct Origin
{
} const ORIGIN;


export struct fPoint
{
    using value_type = float64;

    value_type x{}, y{};

    fPoint()
    {
    }

    fPoint(double x, double y):
        x{ x }, y{ y }
    {
    }

    fPoint(Point p):
        x{ static_cast<value_type>(p.x) },
        y{ static_cast<value_type>(p.y) }
    {
    }

    explicit fPoint(const fVector& v);

    bool operator==(const fPoint& rhs) const
    {
        return isEqual(x, rhs.x) && isEqual(y, rhs.y);
    }

    fPoint& operator+=(const fVector& vect);
};


export struct Vector
{
    using value_type = int32;

    value_type dx{}, dy{};

    bool operator==(const Vector&) const = default;

    Vector() {}

    Vector(const Vector&) = default;

    Vector(value_type dx, value_type dy):
        dx(dx), dy(dy)
    {
    }

    explicit Vector(const Point& p):
        dx(p.x),
        dy(p.y)
    {
    }

    Point asPoint() const
    {
        return { dx, dy };
    }

    Vector& operator+=(const Vector& v)
    {
        dx += v.dx;
        dy += v.dy;
        return *this;
    }

    friend Vector operator*(const Vector& a, double f)
    {
        return { round(a.dx * f), round(a.dy * f) };
    }
};


export struct fVector
{
    using value_type = float64;

    value_type dx{}, dy{};

    fVector()
    {
    }

    fVector(value_type dx, value_type dy):
        dx{ dx }, dy{ dy }
    {
    }

    fVector(const fVector& v) = default;

    fVector(const Vector& v):
        dx{ static_cast<value_type>(v.dx) },
        dy{ static_cast<value_type>(v.dy) }
    {
    }

    explicit fVector(const Point& v):
        dx{ static_cast<value_type>(v.x) },
        dy{ static_cast<value_type>(v.y) }
    {
    }

    float64 Length() const
    {
        return std::sqrt(dx * dx + dy * dy);
    }

    bool operator==(const fVector& rhs) const
    {
        return isEqual(dx, rhs.dx) && isEqual(dy, rhs.dy);
    }

    fVector Perpendicular() const
    {
        return { -dy, dx };
    }

    fVector& operator+=(const fVector& v)
    {
        dx += v.dx;
        dy += v.dy;
        return *this;
    }

    friend fVector operator*(const fVector& a, double f)
    {
        return { a.dx * f, a.dy * f };
    }

    friend Vector round(const fVector& p)
    {
        return { round(p.dx), round(p.dy) };
    }
};


// definitions of member functions


inline fPoint::fPoint(const fVector& v):
    x{ v.dx }, y{ v.dy }
{
}


inline Point& Point::operator+=(const Vector& v)
{
    x += v.dx;
    y += v.dy;
    return *this;
}


inline fPoint& fPoint::operator+=(const fVector& v)
{
    x += v.dx;
    y += v.dy;
    return *this;
}


inline Point& Point::Move(const Vector& v)
{
    x += v.dx;
    y += v.dy;
    return *this;
}


// non-member functions

export {

inline Point point(int32 x, int32 y)
{
    return { x, y };
}

inline fPoint point(float64 x, float64 y)
{
    return { x, y };
}


inline Vector vector(int32 dx, int32 dy)
{
    return { dx, dy };
}

inline fVector vector(float64 dx, float64 dy)
{
    return { dx, dy };
}


inline Point operator+(const Point& a, const Vector& b)
{
    return { a.x + b.dx, a.y + b.dy };
}

inline fPoint operator+(const fPoint& a, const fVector& b)
{
    return { a.x + b.dx, a.y + b.dy };
}

inline Point operator+(const Vector& a, const Point& b)
{
    return { a.dx + b.x, a.dy + b.y };
}

inline fPoint operator+(const fVector& a, const fPoint& b)
{
    return { a.dx + b.x, a.dy + b.y };
}

inline Vector operator+(const Vector& a, const Vector& b)
{
    return { a.dx + b.dx, a.dy + b.dy };
}

inline fVector operator+(const fVector& a, const fVector& b)
{
    return { a.dx + b.dx, a.dy + b.dy };
}

inline fPoint operator+(const Origin&, const fVector& b)
{
    return { b.dx, b.dy };
}



inline Vector operator-(const Point& a, const Point& b)
{
    return { a.x - b.x, a.y - b.y };
}

inline fVector operator-(const fPoint& a, const fPoint& b)
{
    return { a.x - b.x, a.y - b.y };
}

inline fPoint operator-(const fPoint& a, const fVector& b)
{
    return { a.x - b.dx, a.y - b.dy };
}

inline Vector operator-(const Vector& a, const Vector& b)
{
    return { a.dx - b.dx, a.dy - b.dy };
}

inline fVector operator-(const fVector& a, const fVector& b)
{
    return { a.dx - b.dx, a.dy - b.dy };
}

inline Vector operator-(const Point& a, const Origin&)
{
    return { a.x, a.y };
}

inline Vector operator-(const Origin&, const Point& b)
{
    return { -b.x, -b.y };
}

inline Point operator-(const Origin&, const Vector& b)
{
    return { -b.dx, -b.dy };
}

inline fVector operator-(const fPoint& a, const Origin&)
{
    return { a.x, a.y };
}

inline fVector operator-(const Origin&, const fPoint& b)
{
    return { -b.x, -b.y };
}

inline fPoint operator-(const Origin&, const fVector& b)
{
    return { -b.dx, -b.dy };
}



inline float64 SquareDistance(const fPoint& p1, const fPoint& p2)
{
    auto v = fVector{ p2 - p1 };
    return v.dx * v.dx + v.dy * v.dy;
}


inline float64 Distance(const fPoint& p1, const fPoint& p2)
{
    return std::sqrt(SquareDistance(p1, p2));
}


inline Point round(const fPoint& p)
{
    return Point{ round(p.x), round(p.y) };
}


inline auto Length(const fVector& v)
{
    return v.Length();
}


inline fPoint Swap(const fPoint& p)
{
    return { p.y, p.x };
}

}

}
