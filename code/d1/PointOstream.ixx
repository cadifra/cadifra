/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.PointOstream;

import d1.Point;

import std;


export namespace d1
{

inline std::ostream& operator<<(std::ostream& s, const Point& p)
{
    s << "(" << p.x << "," << p.y << ")";
    return s;
}

inline std::ostream& operator<<(std::ostream& s, const fPoint& p)
{
    s << "(" << p.x << "," << p.y << ")";
    return s;
}

inline std::ostream& operator<<(std::ostream& s, const fVector& p)
{
    s << "(" << p.dx << "," << p.dy << ")";
    return s;
}

}
