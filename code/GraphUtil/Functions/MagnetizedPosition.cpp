/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module GraphUtil.Functions;

import GraphUtil.Shapes;

import d1.Rect;


namespace GraphUtil
{

d1::Point magnetizedPosition(
    const d1::Size& size_of_attaching_rect,
    const d1::Point& actual_mouse_pos,
    const d1::Point& drag_offset,
    const d1::nRect& target)
{
    using f8P = d1::fPoint;

    auto poly = ClosedPolygon{};
    poly.push_back(target.topLeft());
    poly.push_back(target.topRight());
    poly.push_back(target.bottomRight());
    poly.push_back(target.bottomLeft());

    auto h_nr = poly.findNearestPoint(actual_mouse_pos, /*horizontal*/ true);
    auto v_nr = poly.findNearestPoint(actual_mouse_pos, /*horizontal*/ false);

    const bool use_vertical_edges =
        d1::squareDistance(h_nr.first, actual_mouse_pos) <
        d1::squareDistance(v_nr.first, actual_mouse_pos);

    auto& nr = use_vertical_edges ? h_nr : v_nr;

    if (use_vertical_edges)
    {
        if (nr.second.dx > 0) // at right edge
            return round(f8P{
                nr.first.x,
                nr.first.y - drag_offset.y });
        else // at left edge
            return round(f8P{
                nr.first.x - size_of_attaching_rect.w,
                nr.first.y - drag_offset.y });
    }
    else
    {
        if (nr.second.dy > 0) // at top edge
            return round(f8P{
                nr.first.x - drag_offset.x,
                nr.first.y + size_of_attaching_rect.h });
        else
            return round(f8P{ // at bottom edge
                nr.first.x - drag_offset.x,
                nr.first.y });
    }
}

}
