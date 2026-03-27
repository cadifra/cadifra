/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module GraphUtil.Shapes;

import d1.Range;

import std;


namespace GraphUtil
{

using std::ranges::for_each;

namespace
{
using C = ClosedPolygon;
}


void C::assign(const d1::nRect& r)
{
    clear();
    reserve(4);
    /*1*/ push_back(r.topLeft());
    /*2*/ push_back(r.topRight());
    /*3*/ push_back(r.bottomRight());
    /*4*/ push_back(r.bottomLeft());
}


namespace
{

struct PointOnPolygon
{
    d1::fPoint pt;
    d1::float64 dist;
    d1::fVector normal;
};

bool smallerDist(const PointOnPolygon& a, const PointOnPolygon& b)
{
    return a.dist < b.dist;
}

}


auto C::findNearestPoint(const d1::fPoint& p, bool horizontal) const
    -> NearestRes
{
    D1_ASSERT(size() > 1);

    const auto intersection =
        horizontal ? &Segment::findHorizontalIntersection
                   : &Segment::findVerticalIntersection;

    auto res = std::vector<PointOnPolygon>{};
    res.reserve(size());
    auto pres = std::vector<PointOnPolygon>{};
    pres.reserve(size()); // preferred results

    auto end = this->end();

    auto a = begin();
    auto b = a + 1;

    for (; a != end; ++a)
    {
        const auto seg = Segment{ *a, *b };
        const auto t = std::invoke(intersection, seg, p);
        const auto pop = PointOnPolygon{ t, d1::distance(t, p), seg.normal() };

        if ((horizontal and t.y == p.y) or (not horizontal and t.x == p.x))
            pres.push_back(pop);
        else
            res.push_back(pop);

        if (++b == end)
            b = begin();
    }

    D1_ASSERT(pres.size() or res.size());

    auto& xres = (pres.size() ? pres : res);

    size_type i = std::ranges::min_element(xres, smallerDist) - xres.begin();

    return { xres.at(i).pt, xres.at(i).normal };
}



auto C::findNearestPoint(const d1::fPoint& p,
    bool horizontal, bool vertical) const -> NearestRes
{
    if (horizontal)
        return findNearestPoint(p, true);
    else if (vertical)
        return findNearestPoint(p, false);

    D1_ASSERT(not horizontal and not vertical);

    auto hres = findNearestPoint(p, true);
    auto vres = findNearestPoint(p, false);

    auto hdist = d1::squareDistance(hres.first, p);
    auto vdist = d1::squareDistance(vres.first, p);

    return (hdist < vdist ? hres : vres);
}


Segment C::getSegment(C::size_type index) const
{
    D1_ASSERT(size() > 1);
    D1_ASSERT(index < size());

    if (index == size() - 1)
        return { back(), front() };
    else
        return { at(index + 1), at(index) };
}


bool C::isOnBoundary(const d1::fPoint& p) const
{
    auto endseg = endSegment();

    for (auto i = beginSegment(); i != endseg; ++i)
        if ((*i).onBoundary(p))
            return true;

    return false;
}


d1::fPoint C::findNewPoint(const ClosedPolygon& oldPolygon,
    const d1::fPoint& oldPoint) const
{
    const auto& strechedPolygon = *this;

    D1_ASSERT(oldPolygon.size() > 1);

    if (oldPolygon.size() != strechedPolygon.size())
    {
        // ## error: polygon changed form. FindNewPoint can not be used.
        D1_ASSERT(0);
        return oldPoint;
    }

    if (strechedPolygon.isOnBoundary(oldPoint))
    {
        // although this polygon might have changed, the point happens to be
        // on the new polygon, so don't try to make things worser if they are
        // perfect!
        return oldPoint;
    }

    // Find the index of the segment with "oldPoint".

    auto dist = std::vector<d1::float64>{};

    auto a = [&](auto s) { dist.push_back(s.squareDistance(oldPoint)); };
    for_each(oldPolygon.beginSegment(), oldPolygon.endSegment(), a);

    auto res = std::ranges::min_element(dist);
    D1_ASSERT(res != dist.end());

    auto index = res - dist.begin();

    const auto newSegment = strechedPolygon.getSegment(index);

    if (newSegment.isHorizontal() or newSegment.isVertical())
    {
        auto p = oldPoint;
        d1::range(newSegment.first.x, newSegment.second.x).limit(p.x);
        d1::range(newSegment.first.y, newSegment.second.y).limit(p.y);
        return p;
    }

    const auto oldSegment = oldPolygon.getSegment(index);

    auto vs = newSegment.second - newSegment.first;
    auto vo = oldSegment.second - oldSegment.first;

    if (std::fabs(vo.dx) > std::fabs(vo.dy))
        return newSegment.first + vs * ((oldPoint.x - oldSegment.first.x) / vo.dx);

    return newSegment.first + vs * ((oldPoint.y - oldSegment.first.y) / vo.dy);
}


namespace
{
class SegmentIter: public d1::Iterator<Segment>::Imp
{
    using Base = d1::Iterator<Segment>::Imp;

    using Vect = std::vector<d1::fPoint>;
    using It = Vect::const_iterator;

    const Vect& vect_;
    It it_;

public:
    SegmentIter(const Vect& vect, It it):
        vect_{ vect }, it_{ it }
    {
    }

    auto clone() const -> std::unique_ptr<Base>
    {
        return std::make_unique<SegmentIter>(vect_, it_);
    }

    bool isEqual(const Base& base_rhs) const
    {
        auto rhs = static_cast<const SegmentIter&>(base_rhs);
        return (it_ == rhs.it_) and (&vect_ == &rhs.vect_);
    }

    void next() { ++it_; }

    Segment val() const
    {
        auto itn = it_ + 1;
        if (itn == vect_.end())
            itn = vect_.begin();
        return { *it_, *itn };
    }
};
}


auto C::beginSegment() const -> d1::Iterator<Segment>
{
    return d1::Iterator<Segment>{
        std::make_unique<SegmentIter>(*this, begin())
    };
}


auto C::endSegment() const -> d1::Iterator<Segment>
{
    return d1::Iterator<Segment>{
        std::make_unique<SegmentIter>(*this, end())
    };
}

}
