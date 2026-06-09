/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module GraphUtil.Segment;

import d1.Point;

namespace GraphUtil
{

export class Segment
//
//  a segment includes all points on a line between (and including)
//  two points. The length of a segment is the distance from its
//  first to its second point.
//
{
public:
    d1::fPoint first, second;

    bool isHorizontal() const;
    bool isVertical() const;

    bool onBoundary(const d1::fPoint& p) const;
    // returns true if p is on this segment.

    d1::float64 squareDistance(const d1::fPoint& p) const;
    // returns the square of the smallest distance of p to any point q of
    // the segment.

    d1::fPoint findHorizontalIntersection(const d1::fPoint& p) const;
    d1::fPoint findVerticalIntersection(const d1::fPoint& p) const;

    bool findIntersection(
        const d1::fPoint& p1, const d1::fPoint& p2,
        d1::fPoint& result) const;
    // checks if infinite line through (p1,p2) intersects this segment.
    // If yes sets result to intersecting point and returns true.
    // If no result is not changed and returns false.

    d1::fVector normal() const;
};

}

module : private;

import GraphUtil.Line;
    
import d1.isEqual;
import d1.Range;

import std;


namespace GraphUtil
{

using C = Segment;


bool C::isHorizontal() const
{
    return GraphUtil::isHorizontal(first, second);
}


bool C::isVertical() const
{
    return GraphUtil::isVertical(first, second);
}


bool C::onBoundary(const d1::fPoint& p) const
{
    return d1::isEqual(squareDistance(p), 0.0);
}


d1::float64 C::squareDistance(const d1::fPoint& p) const
{
    auto u = second - first;
    auto r = p - first;

    const d1::float64 A = u.dx * u.dx + u.dy * u.dy;
    const d1::float64 B = r.dx * u.dx + r.dy * u.dy;

    if (A == 0 or B < 0)
        return d1::squareDistance(first, p);

    if (B > A)
        return d1::squareDistance(second, p);

    const d1::float64 C = u.dy * r.dx - u.dx * r.dy;

    return C * C / A;
}


d1::fPoint C::findHorizontalIntersection(const d1::fPoint& p) const
{
    if (isHorizontal())
        return std::fabs(first.x - p.x) < std::fabs(second.x - p.x) ? first : second;

    if (isVertical())
    {
        d1::float64 y = p.y;
        d1::range(first.y, second.y).limit(y);
        return { first.x, y };
    }

    const bool swap = first.y < second.y;

    auto high = swap ? second : first;
    auto low = swap ? first : second;

    if (p.y <= low.y)
        return low;

    if (p.y >= high.y)
        return high;

    return {
        low.x + (high.x - low.x) * ((p.y - low.y) / (high.y - low.y)),
        p.y
    };
}


d1::fPoint C::findVerticalIntersection(const d1::fPoint& p) const
{
    using d1::swap;

    const auto s = Segment{ swap(first), swap(second) };
    return swap(s.findHorizontalIntersection(swap(p)));
}


bool C::findIntersection(
    const d1::fPoint& p1, const d1::fPoint& p2,
    d1::fPoint& result) const
{
    return false;
}


d1::fVector C::normal() const
{
    auto vec = second - first;
    return { -vec.dy, vec.dx };
}

}
