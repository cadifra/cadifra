/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core.Attach;

namespace Core
{

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
