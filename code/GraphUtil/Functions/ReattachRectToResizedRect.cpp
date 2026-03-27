/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module GraphUtil.Functions;

import d1.Rect;


namespace GraphUtil
{

d1::Point reattachRectToResizedRect(
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
