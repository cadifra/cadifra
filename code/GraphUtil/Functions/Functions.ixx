/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module GraphUtil.Functions;

import d1.Rect;


namespace GraphUtil
{

// Calculates the position of a moving rect ("attaching rect") that must
// be magnetized to fixed-size-fixed-position rect
//
export d1::Point magnetizedPosition(
    const d1::Size& size_of_attaching_rect,
    const d1::Point& actual_mouse_pos,
    const d1::Point& drag_offset, // drag_point relative to top left
                                  // edge of attaching rect
    const d1::nRect& target);

//
//  If attached_rect was attached to old_terminal_box, and old_terminal_box has been
//  resized to new_target_box, then
//
//  reattachRectToResizedRect() returns the new position for attached_rect, such that
//  attached_rect is again maximally attached to new_terminal_box.
//
export d1::Point reattachRectToResizedRect(
    const d1::nRect& old_terminal_box,
    const d1::nRect& new_terminal_box,
    const d1::nRect& attached_rect);

}
