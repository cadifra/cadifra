/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Rotate;

import d1.Rect;


export namespace d1
{

// Coordinate transformations.
// Rotations around (0,0), in steps of 90 degrees

template <class P>
P rotatePointLeft90(const P& p) { return { -p.y, p.x }; }

template <class P>
P rotatePointRight90(const P& p) { return { p.y, -p.x }; }

template <class P>
P rotatePoint180(const P& p) { return { -p.x, -p.y }; }


// Rect

template <class R>
R rotateRectLeft90(const R& r)
{
    return {
        rotatePointLeft90(r.topLeft()),
        rotatePointLeft90(r.bottomRight())
    };
}

template <class R>
R rotateRectRight90(const R& r)
{
    return {
        rotatePointRight90(r.topLeft()),
        rotatePointRight90(r.bottomRight())
    };
}

template <class R>
R rotateRect180(const R& r)
{
    return {
        rotatePoint180(r.topLeft()),
        rotatePoint180(r.bottomRight())
    };
}

// overloads

Point rotateLeft90(const Point& p)
{
    return rotatePointLeft90(p);
}

nRect rotateLeft90(const nRect& r)
{
    return rotateRectLeft90(r);
}

Point rotateRight90(const Point& p)
{
    return rotatePointRight90(p);
}

nRect rotateRight90(const nRect& r)
{
    return rotateRectRight90(r);
}

Point rotate180(const Point& p)
{
    return rotatePoint180(p);
}

nRect rotate180(const nRect& r)
{
    return rotateRect180(r);
}

}
