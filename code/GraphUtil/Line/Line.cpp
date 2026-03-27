/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <stdlib.h>
#include <math.h>

module GraphUtil.Line;

import d1.isEqual;
import d1.Rect;
import d1.round;


namespace GraphUtil
{

bool isNear(
    const d1::Point& theLineStart, const d1::Point& theLineEnd,
    const d1::Point& theTestPoint,
    d1::int32 d)
{
    auto encl_rect = nrect(theLineStart, theLineEnd).enlarge(d);

    if (not encl_rect.encloses(theTestPoint)) // quick negative test (a speedup)
        return false;

    if (theLineStart == theLineEnd) // avoid mathematical problems
        return true;

    auto u = theLineEnd - theLineStart;
    auto r = theTestPoint - theLineStart;

    double ux = u.dx;
    double uy = u.dy;
    double rx = r.dx;
    double ry = r.dy;

    double A = ux * ux + uy * uy;
    double B = rx * ux + ry * uy;
    double S = d * sqrt(A);

    if (B < -S)
        return false;

    if ((B - A) > S)
        return false;

    double C = fabs(ry * ux - rx * uy);

    if (C > S)
        return false;

    return true;
}


bool isNear(
    const d1::fPoint& theLineStart, const d1::fPoint& theLineEnd,
    const d1::fPoint& theTestPoint,
    d1::int32 d)
{
    auto encl_rect = nrect(theLineStart, theLineEnd).enlarge(d);

    if (not encl_rect.encloses(theTestPoint)) // quick negative test (a speedup)
        return false;

    if (theLineStart == theLineEnd) // avoid mathematical problems
        return true;

    auto u = theLineEnd - theLineStart;
    auto r = theTestPoint - theLineStart;

    double ux = u.dx;
    double uy = u.dy;
    double rx = r.dx;
    double ry = r.dy;

    double A = ux * ux + uy * uy;
    double B = rx * ux + ry * uy;
    double S = d * sqrt(A);

    if (B < -S)
        return false;

    if ((B - A) > S)
        return false;

    double C = fabs(ry * ux - rx * uy);

    if (C > S)
        return false;

    return true;
}


bool isNearInfinite(
    const d1::fPoint& theLineStart, const d1::fPoint& theLineEnd,
    const d1::fPoint& theTestPoint,
    d1::int32 d)
{
    if (theLineStart == theLineEnd) // avoid mathematical problems
        return true;

    auto u = theLineEnd - theLineStart;
    auto r = theTestPoint - theLineStart;

    double ux = u.dx;
    double uy = u.dy;
    double rx = r.dx;
    double ry = r.dy;

    double A = ux * ux + uy * uy;
    double S = d * sqrt(A);
    double C = fabs(ry * ux - rx * uy);

    if (C > S)
        return false;

    return true;
}


bool isLeft(const d1::Point& a, const d1::Point& b, const d1::Point& p)
{
    auto u = b - a;
    auto r = p - a;
    double q2_times_v = r.dy * u.dx - r.dx * u.dy;
    return q2_times_v > 0;
}


bool isHorizontal(const d1::Point& a, const d1::Point& b)
{
    return (a.y == b.y) and (a.x != b.y);
}


bool isVertical(const d1::Point& a, const d1::Point& b)
{
    return (a.x == b.x) and (a.y != b.y);
}


bool isHorizontal(const d1::fPoint& a, const d1::fPoint& b)
{
    return d1::isEqual(a.y, b.y) and not d1::isEqual(a.x, b.x);
}


bool isVertical(const d1::fPoint& a, const d1::fPoint& b)
{
    return d1::isEqual(a.x, b.x) and not d1::isEqual(a.y, b.y);
}


d1::Point magnetize(
    const d1::Point& theLineStart, const d1::Point& theLineEnd,
    const d1::Point& theMagnetizePoint)
{
    auto u = theLineEnd - theLineStart;
    auto r = theMagnetizePoint - theLineStart;

    double ux = u.dx;
    double uy = u.dy;
    double rx = r.dx;
    double ry = r.dy;

    double A = rx * ux + ry * uy;
    double B = ux * ux + uy * uy;

    double C = A / B;

    if (C > 1.0)
        C = 1.0;

    if (C < 0.0)
        C = 0.0;

    double q1x = ux * C;
    double q1y = uy * C;

    return d1::vector(d1::round(q1x), d1::round(q1y)) + theLineStart;
}


d1::fPoint magnetize(
    const d1::fPoint& theLineStart, const d1::fPoint& theLineEnd,
    const d1::fPoint& theMagnetizePoint)
{
    auto u = theLineEnd - theLineStart;
    auto r = theMagnetizePoint - theLineStart;

    auto ux = u.dx;
    auto uy = u.dy;
    auto rx = r.dx;
    auto ry = r.dy;

    auto A = rx * ux + ry * uy;
    auto B = ux * ux + uy * uy;

    auto C = A / B;

    if (C > 1.0)
        C = 1.0;

    if (C < 0.0)
        C = 0.0;

    auto q1x = ux * C;
    auto q1y = uy * C;

    return theLineStart + d1::vector(q1x, q1y);
}

}
