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


auto C::FindNearestPoint(const ShiftSet& dss,
    const d1::fPoint& p, bool horizontal) const -> NearestRes
{
    if (!dss.IsDeferredShifting(*this))
        return FindNearestPointImpl(p, horizontal);

    auto& v = dss.GetDeferredShift();
    auto sp = p;
    if (v.shiftX)
        sp.x -= v.distance.dx;
    if (v.shiftY)
        sp.y -= v.distance.dy;

    auto res = FindNearestPointImpl(sp, horizontal);

    if (v.shiftX)
        res.first.x += v.distance.dx;
    if (v.shiftY)
        res.first.y += v.distance.dy;

    return res;
}


d1::fPoint C::Nearest(const d1::fPoint& p) const
{
    auto p1 = FindNearestPointImpl(p, true).first;
    auto p2 = FindNearestPointImpl(p, false).first;

    if (Length(p1 - p) < Length(p2 - p))
        return p1;
    else
        return p2;
}

}
