/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module GraphUtil.Shapes;

import d1.Iterator;
import d1.Rect;

import std;


namespace GraphUtil
{

export class Ellipse
{
    d1::fnRect rect_;

public:
    Ellipse(const d1::fnRect& r):
        rect_{ r }
    {
    }

    using NearestRes = std::pair<
        d1::fPoint, // nearest point on envelope
        d1::fVector // perpendicular to envelope, in outside
        >;               // direction

    auto findNearestPoint(const d1::fPoint& p, bool horizontal) const
        -> NearestRes;

    d1::fPoint findNewPoint(const Ellipse& oldEllipse,
        const d1::fPoint& oldPoint) const;
    // *this = stretchedEllipse
    // "oldPoint" is a point on the boundary of "oldEllipse".
    // FindNewPoint returns the point on the boundary of "stretchedEllipse" that
    // corresponds to "oldPoint" on the boundary of "oldEllipse".
};


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


// PRE: the vector contains the points of the polygon in consecutive
// clockwise order
//
export class ClosedPolygon: public std::vector<d1::fPoint>
{
public:
    ClosedPolygon() {}
    ClosedPolygon(const d1::nRect& r) { assign(r); }

    void assign(const d1::nRect&);

    auto beginSegment() const -> d1::Iterator<Segment>;
    auto endSegment() const -> d1::Iterator<Segment>;

    Segment getSegment(size_type index) const;
    // PRE: (size() > 1) and (index < p.size()

    bool isOnBoundary(const d1::fPoint& p) const;
    // return true if p is on the boundary of this polygon

    using NearestRes = std::pair<
        d1::fPoint, // nearest point on envelope
        d1::fVector // perpendicular to envelope, in outside
        >;               // direction

    auto findNearestPoint(const d1::fPoint& p, bool horizontal) const
        -> NearestRes;
    // Finds the nearest intersection of the polygon with the horizontal (or
    // vertical if "horizontal" is false) line through p.
    // If no such intersection exists, FindNearestPoint returns the nearest point.

    auto findNearestPoint(const d1::fPoint& p, bool horizontal, bool vertical) const
        -> NearestRes;
    // only horizontal or vertical may be true, but not both. If both are false
    // then the intersecting line is considered oblique.

    d1::fPoint findNewPoint(const ClosedPolygon& oldPolygon,
        const d1::fPoint& oldPoint) const;
    // *this = strechedPolygon
    // "oldPoint" is a point on the boundary of "oldPolygon". "strechedPolygon"
    // is similar to "oldPolygon" (oldPolygon.size() == strechedPolygon.size()).
    // FindNewPoint returns the point on the boundary of "strechedPolygon" that
    // corresponds to "oldPoint" on the boundary of "oldPolygon".
    // If oldPolygon.size() != strechedPolygon.size() FindNewPoint returns
    // oldPoint.
    // Do not call FindNewPoint if oldPolygon.size() != strechedPolygon.size().
};


export class RoundRect
{
    d1::nRect rect_;
    d1::float64 radius_;
    ClosedPolygon poly_;

public:
    RoundRect(const d1::nRect& rect, d1::float64 rad):
        rect_{ rect }, radius_{ rad }, poly_{ rect }
    {
    }

    using NearestRes = std::pair<
        d1::fPoint, // nearest point on envelope
        d1::fVector // perpendicular to envelope, in outside
        >;               // direction

    auto findNearestPoint(const d1::fPoint& p, bool horizontal) const
        -> NearestRes;

    d1::fPoint findNewPoint(
        const d1::nRect& oldRect,
        const d1::fPoint& oldPoint) const;
    // *this = stretchedRoundRect
    // "oldPoint" is a point on the boundary of "OldRoundRect" defined by
    // oldRect (radius unchanged).
    // FindNewPoint returns the point on the boundary of "stretchedRoundRect" that
    // corresponds to "oldPoint" on the boundary of "OldRoundRect".

private:
    bool edgeCorrection(NearestRes& res, bool horizontal) const;
    bool edgeCorrectionNewPoint(d1::fPoint& res,
        const d1::nRect& oldRect, const d1::fPoint& oldPoint) const;
};

}
