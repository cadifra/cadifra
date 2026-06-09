/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.ITerminal;

import Core.Shift;

import d1.Point;

import std;


namespace Core
{

export class ITerminal: public virtual IShiftable
//
// IAttachment's can be attached to ITerminal's through template class IPartner<T>.
// ITerminal is also used as base for dynamic_casts to concrete IPartner<T> classes.
//
{
public:
    using NearestRes = std::pair<
        d1::fPoint,   // nearest point on envelope
        d1::fVector>; // perpendicular to envelope, in outside direction

    auto findNearestPoint(const ShiftSet&, const d1::fPoint& p,
        bool horizontal) const -> NearestRes;
    // Finds the nearest intersection of the terminal's boundary with a
    // horizontal (or vertical if "horizontal == false) line through p.
    // If no such intersection exists, FindNearestPoint returns the nearest
    // point on the boundary.

    virtual auto findNearestPointImpl(const d1::fPoint& p,
        bool horizontal) const -> NearestRes = 0;

    d1::fPoint nearest(const d1::fPoint& p) const;

    class ReshapeInfo;
};


class ITerminal::ReshapeInfo
{
public:
    auto getNewAttachmentPos(const d1::fPoint& oldPos,
        const d1::fPoint& farPos, const ShiftSet&) const -> d1::fPoint;
    // calls GetNewAttachmentPosImpl

private:
    virtual auto getNewAttachmentPosImpl(const d1::fPoint& oldPos,
        const d1::fPoint& farPos) const -> d1::fPoint = 0;

public:
    virtual auto getReshapedTerminal() const -> const ITerminal& = 0;

protected:
    ~ReshapeInfo() = default;
};


auto ITerminal::findNearestPoint(const ShiftSet& dss,
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


d1::fPoint ITerminal::nearest(const d1::fPoint& p) const
{
    auto p1 = findNearestPointImpl(p, true).first;
    auto p2 = findNearestPointImpl(p, false).first;

    if (length(p1 - p) < length(p2 - p))
        return p1;
    else
        return p2;
}


auto ITerminal::ReshapeInfo::getNewAttachmentPos(
    const d1::fPoint& oldPos, const d1::fPoint& farPos,
    const ShiftSet& dss) const -> d1::fPoint
{
    auto res = getNewAttachmentPosImpl(oldPos, farPos);

    if (not dss.isDeferredShifting(getReshapedTerminal()))
        return res;

    auto v = dss.getDeferredShift();
    if (v.shiftX)
        res.x += v.distance.dx;
    if (v.shiftY)
        res.y += v.distance.dy;

    return res;
}

}
