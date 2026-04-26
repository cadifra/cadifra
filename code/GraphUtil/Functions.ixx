/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module GraphUtil.Functions;

import GraphUtil.Shapes;

import d1.Rect;


namespace GraphUtil
{

// Calculates the position of a moving rect ("attaching rect") that must
// be magnetized to fixed-size-fixed-position rect
//
export d1::Point magnetizedPosition(
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


//
//  If attached_rect was attached to old_terminal_box, and old_terminal_box has been
//  resized to new_target_box, then
//
//  reattachRectToResizedRect() returns the new position for attached_rect, such that
//  attached_rect is again maximally attached to new_terminal_box.
//
export d1::Point reattachRectToResizedRect(
    const d1::nRect& otb, // old_terminal_box
    const d1::nRect& ntb, // new_terminal_box
    const d1::nRect& ar   // attached_rect
)
{
    const auto s = ar.size();

    if (otb.b == ar.t)
    {
        // ## attached_rect was at bottom edge
        if (ar.l < ntb.l)
            return { ntb.l, ntb.b };
        else if (ar.r > ntb.r)
            return { ntb.r - s.w, ntb.b };
        else
            return { ar.l, ntb.b };
    }

    else if (otb.r == ar.l)
    {
        // ## attached_rect was at right edge
        if (ar.t > ntb.t)
            return { ntb.r, ntb.t };
        else if (ar.b < ntb.b)
            return { ntb.r, ntb.b + s.h };
        else
            return { ntb.r, ar.t };
    }

    else if (otb.t == ar.b)
    {
        // ## attached_rect was at top edge
        if (ar.l < ntb.l)
            return { ntb.l, ntb.t + s.h };
        else if (ar.r > ntb.r)
            return { ntb.r - s.w, ntb.t + s.h };
        else
            return { ar.l, ntb.t + s.h };
    }

    else if (otb.l == ar.r)
    {
        // ## attached_rect was at left edge
        if (ar.t > ntb.t)
            return { ntb.l - s.w, ntb.t };
        else if (ar.b < ntb.b)
            return { ntb.l - s.w, ntb.b + s.h };
        else
            return { ntb.l - s.w, ar.t };
    }

    else
    {
        // ## attached_rect was not attached to old_terminal_box
        return pos(ar);
    }
}

}
