/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Canvas;

import Canvas.AdjustMarkerInfo;
import Canvas.Brush;
import Canvas.Group;
import Canvas.ICustomDrawer;
import Canvas.IElementImp;
import Canvas.Order;
import Canvas.PageDefaults;
import Canvas.PageInfo;
import Canvas.PictureDescription;

import d1.Rect;

import std;


namespace Canvas
{

export using PolyPoints = std::vector<d1::fPoint>;


export class Canvas
{
public:
    virtual ~Canvas() = default;

    virtual void adjustMarker(Group&, const d1::fPoint& pos,
        const AdjustMarkerInfo& i, bool isTarget = true) = 0;

    virtual void line(Group&, const d1::fPoint& a, const d1::fPoint& b) = 0;

    virtual void dashedLine(Group&, const d1::fPoint& a, const d1::fPoint& b,
        bool red = false) = 0;

    virtual void alternateLine(Group&, const d1::fPoint& a,
        const d1::fPoint& b) = 0;

    virtual void ellipse(Group&, const d1::fnRect& r) = 0;

    virtual void ellipseFiller(Group&, const d1::fnRect& r, const Brush& b,
        Order order) = 0;

    virtual void dashedEllipse(Group&, const d1::fnRect& r, bool red = false) = 0;

    virtual void rect(Group&, const d1::fnRect& r) = 0;

    virtual void rectFiller(Group&, const d1::fnRect& r, const Brush& b, Order order) = 0;

    virtual void dashedRect(Group&, const d1::fnRect& r, bool red = false) = 0;

    virtual void roundRect(Group&, const d1::fnRect& r,
        const d1::float64& width, const d1::float64& height) = 0;

    virtual void roundRectFiller(Group&, const d1::fnRect& r,
        const d1::float64& width, const d1::float64& height,
        const Canvas::Brush& b, Order order) = 0;

    virtual void closedPolygon(Group&, PolyPoints& pp /* empty on return! */) = 0;

    virtual void closedPolygonFiller(Group&, PolyPoints& pp, /* empty on return! */
        const Brush& b, Order order) = 0;

    virtual void picture(Group&, const d1::fPoint& center, PictureDescription pd) = 0;

    virtual void custom(Group&, std::unique_ptr<ICustomDrawer>) = 0;

    virtual d1::fnRect getLogicBounds() = 0;

    virtual void setPageInfo(const PageInfo&) = 0;
    virtual const PageInfo& getPageInfo() const = 0;

protected:
    Canvas() = default;

    Canvas(const Canvas&) = delete;
    Canvas& operator=(const Canvas&) = delete;
};


export void addBoxAdjustMarkers(Canvas& c, Group&, const d1::fnRect& r,
    bool isTarget = true);


export void addBoxMidPointsAdjustMarkers(Canvas& c, Group&, const d1::fnRect& r,
    bool isTarget = true);

}
