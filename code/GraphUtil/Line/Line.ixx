/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module GraphUtil.Line;

import d1.Point;


export namespace GraphUtil
{

bool isNear(
    const d1::Point& theLineStart, const d1::Point& theLineEnd,
    const d1::Point& theTestPoint,
    d1::int32 theDistance);
// Returns true, if theTestPoint is within theDistance near the line
// that is defined by theLineStart and theLineEnd.

bool isNear(
    const d1::fPoint& theLineStart, const d1::fPoint& theLineEnd,
    const d1::fPoint& theTestPoint,
    d1::int32 theDistance);

// Returns true, if theTestPoint is within theDistance near the infinite line
// through the two points theLineStart and theLineEnd.
bool isNearInfinite(
    const d1::fPoint& theLineStart, const d1::fPoint& theLineEnd,
    const d1::fPoint& theTestPoint,
    d1::int32 theDistance);

bool isLeft(const d1::Point& a, const d1::Point& b, const d1::Point& p);
// (a, b) defines an infinite line. Assume you are sitting in a
// looking into the direction of b. IsLeft then returns true, if p
// is on the lefthand side.

bool isHorizontal(const d1::Point& a, const d1::Point& b);
bool isVertical(const d1::Point& a, const d1::Point& b);

bool isHorizontal(const d1::fPoint& a, const d1::fPoint& b);
bool isVertical(const d1::fPoint& a, const d1::fPoint& b);

d1::Point magnetize(
    const d1::Point& theLineStart, const d1::Point& theLineEnd,
    const d1::Point& theMagnetizePoint);

d1::fPoint magnetize(
    const d1::fPoint& theLineStart, const d1::fPoint& theLineEnd,
    const d1::fPoint& theMagnetizePoint);

}
