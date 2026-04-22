/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;

namespace Core
{

namespace
{
using C = ITerminal;
}


auto C::findNearestPoint(const ShiftSet& dss,
    const d1::fPoint& p, bool horizontal) const -> NearestRes
{
    if (not dss.isDeferredShifting(*this))
        return findNearestPointImpl(p, horizontal);

    auto& v = dss.getDeferredShift();
    auto sp = p;
    if (v.shiftX)
        sp.x -= v.distance.dx;
    if (v.shiftY)
        sp.y -= v.distance.dy;

    auto res = findNearestPointImpl(sp, horizontal);

    if (v.shiftX)
        res.first.x += v.distance.dx;
    if (v.shiftY)
        res.first.y += v.distance.dy;

    return res;
}


d1::fPoint C::nearest(const d1::fPoint& p) const
{
    auto p1 = findNearestPointImpl(p, true).first;
    auto p2 = findNearestPointImpl(p, false).first;

    if (length(p1 - p) < length(p2 - p))
        return p1;
    else
        return p2;
}

}
