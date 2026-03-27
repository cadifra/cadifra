/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module GraphUtil.Shapes;


namespace GraphUtil
{

namespace
{
using C = RoundRect;
}


auto C::findNearestPoint(const d1::fPoint& p, bool horizontal) const
    -> NearestRes
{
    auto res = poly_.findNearestPoint(p, horizontal);

    edgeCorrection(res, horizontal);

    return res;
}


d1::fPoint C::findNewPoint(
    const d1::nRect& oldRect,
    const d1::fPoint& oldPoint) const
{
    auto p = oldPoint;

    auto R = radius_;
    auto D = 2 * R;

    if ((p.x > (oldRect.r - R)) and (p.y > (oldRect.t - R)))
    {
        // ## on top right edge
        auto dx = rect_.r - oldRect.r;
        auto dy = rect_.t - oldRect.t;
        return { p.x + dx, p.y + dy };
    }

    if ((p.x < (oldRect.l + R)) and (p.y > (oldRect.t - R)))
    {
        // ## on top left edge
        auto dx = rect_.l - oldRect.l;
        auto dy = rect_.t - oldRect.t;
        return { p.x + dx, p.y + dy };
    }

    if ((p.x < (oldRect.l + R)) and (p.y < (oldRect.b + R)))
    {
        // ## on bottom left edge
        auto dx = rect_.l - oldRect.l;
        auto dy = rect_.b - oldRect.b;
        return { p.x + dx, p.y + dy };
    }

    if ((p.x > (oldRect.r - R)) and (p.y < (oldRect.b + R)))
    {
        // ## on bottom right edge
        auto dx = rect_.r - oldRect.r;
        auto dy = rect_.b - oldRect.b;
        return { p.x + dx, p.y + dy };
    }

    const auto old_poly = ClosedPolygon{ oldRect };

    auto res = poly_.findNewPoint(old_poly, oldPoint);

    edgeCorrectionNewPoint(res, oldRect, oldPoint);

    return res;
}


bool C::edgeCorrection(NearestRes& res, bool horizontal) const
{
    using f8P = d1::fPoint;

    auto p = res.first;

    auto R = radius_;
    auto D = 2 * R;

    if ((p.x > (rect_.r - R)) and (p.y > (rect_.t - R)))
    {
        // ## on top right edge
        auto er = nrect(f8P(rect_.r - D, rect_.t), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, horizontal);
        return true;
    }

    if ((p.x < (rect_.l + R)) and (p.y > (rect_.t - R)))
    {
        // ## on top left edge
        auto er = nrect(f8P(rect_.l, rect_.t), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, horizontal);
        return true;
    }

    if ((p.x < (rect_.l + R)) and (p.y < (rect_.b + R)))
    {
        // ## on bottom left edge
        auto er = nrect(f8P(rect_.l, rect_.b + D), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, horizontal);
        return true;
    }

    if ((p.x > (rect_.r - R)) and (p.y < (rect_.b + R)))
    {
        // ## on bottom right edge
        auto er = nrect(f8P(rect_.r - D, rect_.b + D), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, horizontal);
        return true;
    }

    return false;
}


bool C::edgeCorrectionNewPoint(d1::fPoint& res,
    const d1::nRect& oldRect, const d1::fPoint& oldPoint) const
{
    using f8P = d1::fPoint;

    auto p = res;

    auto R = radius_;
    auto D = 2 * R;

    if ((p.x > (rect_.r - R)) and (p.y > (rect_.t - R)))
    {
        // ## on top right edge
        auto er = nrect(f8P(rect_.r - D, rect_.t), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, true).first;
        return true;
    }

    if ((p.x < (rect_.l + R)) and (p.y > (rect_.t - R)))
    {
        // ## on top left edge
        auto er = nrect(f8P(rect_.l, rect_.t), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, true).first;
        return true;
    }

    if ((p.x < (rect_.l + R)) and (p.y < (rect_.b + R)))
    {
        // ## on bottom left edge
        auto er = nrect(f8P(rect_.l, rect_.b + D), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, true).first;
        return true;
    }

    if ((p.x > (rect_.r - R)) and (p.y < (rect_.b + R)))
    {
        // ## on bottom right edge
        auto er = nrect(f8P(rect_.r - D, rect_.b + D), D, D);
        const auto e = Ellipse{ er };
        res = e.findNearestPoint(p, true).first;
        return true;
    }

    return false;
}

}
