/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.ICanvas;

import Canvas.Base;

import d1.Observer;
import d1.Rect;

import WinUtil.types;

import std;


namespace Canvas
{

export class CaretRef;
export class IScreenCanvas;


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


export class IZoomChange
{
public:
    virtual void zoomNotification(const double& zoomFactor,
        const d1::Point& fixPoint) = 0;
    // The "fixPoint" is in client coordinates an marks the point that
    // didn't change its position during the change of the zoom factor.

protected:
    ~IZoomChange() = default;
};


export class ICaret
{
public:
    virtual ~ICaret() = default;

    virtual void setMaster(CaretRef& master) = 0;
    virtual void forget(CaretRef& master) = 0;
    virtual void destroy() = 0;
};


export class CaretRef
{
    ICaret* caret_; // no ownership, may be null

public:
    CaretRef(ICaret* c = 0);
    ~CaretRef(); // destroys the caret

    void destroy();

    CaretRef(const CaretRef& rhs);
    CaretRef& operator=(const CaretRef& rhs);
};


export class ScrollKeysDisabler
{
public:
    ScrollKeysDisabler(IScreenCanvas&);

    ScrollKeysDisabler(const ScrollKeysDisabler&) = delete;
    ScrollKeysDisabler& operator=(const ScrollKeysDisabler&) = delete;

    ~ScrollKeysDisabler(); // intentionally not virtual

private:
    IScreenCanvas& screenCanvas_;
};


export class IScreenCanvas: public virtual Canvas
{
public:
    virtual void resetViewPoint() = 0;

    virtual d1::Vector getOffset() const = 0;      // client coordinates
    virtual void setOffset(const d1::Vector&) = 0; // client coordinates


    virtual d1::fPoint clientToLogic(const d1::Point&) const = 0;
    // Translates window client coordinates into logical absolute coordinates.

    virtual d1::Point logicToClient(const d1::fPoint&) const = 0;
    // Translates window client coordinates into logical absolute coordinates.

    virtual d1::fPoint screenToLogic(const d1::Point&) const = 0;
    // Translate screen coordinates into logical absolute coordinates.

    virtual d1::Point logicToScreen(const d1::fPoint&) const = 0;
    // Translates logical absolute coordinates into screen coordinates.

    virtual d1::fnRect clientRect() const = 0;
    // Returns the window client rect in logical absolute coordinates.

    virtual void setRasterSize(double millimeters) = 0;
    // Sets the rastersize value that is used to alter the zoom factors
    // to try to get an optimal rendering (n * pixels = k * rastersize).
    // The default value after creation is 1 millimeter.

    virtual double setZoomFactor(
        double factor, const d1::Point& cp = {}) = 0;
    // Sets the zoom factor. cp is the point in client coordinates that
    // will not change its position while changing the zoom factor.
    // Returns the new zoom factor.

    virtual double calculateZoomFactor(double f) const = 0;
    // Calculates the value that setZoomFactor(f) would return,
    // without actually changing the zoom factor of this object.

    virtual double getZoomFactor() const = 0;

    virtual double zoomToFit() = 0;

    virtual double zoomToRect(const d1::fnRect& r) = 0;
    // r in logic coordinates

    virtual void copyZoomFactor(
        IScreenCanvas& sc, const d1::Point& cp = {}) = 0;
    // Same as SetZoomFactor but takes the zoom factor from "sc"

    virtual double incrementZoom(const d1::Point& cp = {}) = 0;
    virtual double decrementZoom(const d1::Point& cp = {}) = 0;

    virtual void scrollTo(const d1::fPoint& p1, const d1::Point& p2) = 0;
    // Scrolls the drawing such that the point p1 (in logic coordinates)
    // lies on the point p2 (in client coordinates).

    virtual Scroller createScroller() = 0;
    // The created Scroller object (with value semantic) allows automatic
    // scrolling.

    virtual Scroller createDragDropScroller() = 0;
    // Same as CreateScroller but the created Scroller object makes no
    // timer generated notification calls and scrolls only if the mouse
    // cursor doesn't exceed a velocity limit (allows dragging an object
    // over the window bounds).

    virtual void synchronousUpdate() = 0;
    // Call SynchronousUpdate if you want to force an immediate update
    // of all accumulated changes.
    // If you do not call this function, all accumulated changes will
    // be executed at the next possible time (within idle processing).

    virtual CaretRef setCaret(
        const d1::fPoint& start, const d1::fPoint& end,
        bool rightToLeft) = 0;

    virtual auto getScrollConnector()
        -> d1::Observer::C<IScrollChange> = 0;

    virtual auto getZoomConnector()
        -> d1::Observer::C<IZoomChange> = 0;

private:
    friend class ScrollKeysDisabler;
    virtual void disableScrollKeys() = 0;
    virtual void enableScrollKeys() = 0;
    // The screen canvas detects 'scroll' keys (home, end, page up/down,
    // left/right/up/down arrow) and scrolls the drawing area if possible.
    // DisableScrollKeys/EnableScrollKeys disables/enables this mechanism.
    // The two functions are cumulative.
};


export class IAbort
{
public:
    virtual bool abort() const = 0;
    // Return true to abort the function that got this interface
    // as a parameter.

protected:
    ~IAbort() = default;
};


export class IPrinterCanvas: public virtual Canvas
{
public:
    virtual void print(
        const IAbort&,
        bool fitToPage,
        bool glueCutMarks,
        const std::pair<int, int>& printRange, // "from" page, "to" page
        const std::wstring& destination,       // "" or a filename,
        bool evaluationMode) = 0;
};


export class IMetafileCanvas: public virtual Canvas
{
public:
    virtual WinUtil::STGMEDIUM createEnhancedMetafile(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual WinUtil::STGMEDIUM createWindowsMetafile(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual WinUtil::STGMEDIUM createBitmap(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual WinUtil::STGMEDIUM createPNG(
        d1::uint32 borderSize /*himetric units*/) const = 0;
    // The caller is responsible to use ::ReleaseStgMedium to delete
    // the returned data.

    virtual void createPNG(
        const wchar_t* filename, d1::uint32 borderSize /*himetric units*/) const = 0;
};

}
