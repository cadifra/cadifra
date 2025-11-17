/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

auto ITerminal::ReshapeInfo::GetNewAttachmentPos(
    const d1::fPoint& oldPos, const d1::fPoint& farPos,
    const ShiftSet& dss) const -> d1::fPoint
{
    auto res = GetNewAttachmentPosImpl(oldPos, farPos);

    if (!dss.IsDeferredShifting(GetReshapedTerminal()))
        return res;

    auto v = dss.GetDeferredShift();
    if (v.shiftX)
        res.x += v.distance.dx;
    if (v.shiftY)
        res.y += v.distance.dy;

    return res;
}

}
