/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Rotate;

export namespace d1
{

// Coordinate transformations.
// Rotations around (0,0), in steps of 90 degrees

template <class P>
P RotatePointLeft90(const P& p) { return { -p.y, p.x }; }

template <class P>
P RotatePointRight90(const P& p) { return { p.y, -p.x }; }

template <class P>
P RotatePoint180(const P& p) { return { -p.x, -p.y }; }


// Rect

template <class R>
R RotateRectLeft90(const R& r)
{
    return {
        RotatePointLeft90(r.TopLeft()),
        RotatePointLeft90(r.BottomRight())
    };
}

template <class R>
R RotateRectRight90(const R& r)
{
    return {
        RotatePointRight90(r.TopLeft()),
        RotatePointRight90(r.BottomRight())
    };
}

template <class R>
R RotateRect180(const R& r)
{
    return {
        RotatePoint180(r.TopLeft()),
        RotatePoint180(r.BottomRight())
    };
}

}
