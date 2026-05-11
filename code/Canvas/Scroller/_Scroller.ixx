/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Scroller;

import d1.Point;

import std;


namespace Canvas
{

export class IScrollChange
{
public:
    virtual void scrollNotification(const d1::fPoint& cursorPosition) = 0;
    // Called by the Scroller to inform that the mouse cursor position has
    // got a new logical value (cursorPosition is in logical coordinates)

protected:
    ~IScrollChange() = default;
};


export class Scroller // has value semantics
{
public:
    Scroller() = default;

    ~Scroller() = default;
    // Calls StopAutoScroll internally.
    // Intentionally not virtual.

    Scroller(const Scroller&) = default;
    Scroller& operator=(const Scroller&) = default;

    d1::fPoint autoScroll(
        const d1::Point& cursorPosition,
        IScrollChange* o = 0);
    // Call this function if you got a new position of the
    // mouse cursor (in WM_MOUSEMOVE or IDropTarget::DragOver).
    // cursorPosition in client coordinates.
    // Calls the optional IScrollChange (even if no scrolling
    // is required).
    // The returned d1::fPoint is the cursorPosition in
    // logical coordinates after a possible scrolling.

    void stopAutoScroll();
    // Cancels any pending scrolling activity

    class IImp;

    Scroller(const std::shared_ptr<IImp>& i);

private:
    std::shared_ptr<IImp> imp_;
};


class Scroller::IImp
{
public:
    virtual d1::fPoint autoScroll(const d1::Point&, IScrollChange*) = 0;
    virtual void stopAutoScroll() = 0;

protected:
    ~IImp() = default;
};

}
