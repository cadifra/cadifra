/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Canvas;

import Canvas.AdjustMarkerInfo;
import Canvas.Brush;
import Canvas.Group;
import Canvas.ICustomDrawer;
import Canvas.Order;
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


module : private;


namespace Canvas
{

void addBoxAdjustMarkers(Canvas& c, Group& g, const d1::fnRect& r, bool isTarget)
{
    using AMI = AdjustMarkerInfo;

    c.adjustMarker(g, r.topLeft(), AMI::LeftUp(), isTarget);
    c.adjustMarker(g, r.topRight(), AMI::RightUp(), isTarget);
    c.adjustMarker(g, r.bottomLeft(), AMI::LeftDown(), isTarget);
    c.adjustMarker(g, r.bottomRight(), AMI::RightDown(), isTarget);
}


void addBoxMidPointsAdjustMarkers(Canvas& c, Group& g, const d1::fnRect& r, bool isTarget)
{
    using AMI = AdjustMarkerInfo;

    auto A = d1::point(r.l + r.width() / 2.0, r.t);
    auto B = d1::point(A.x, r.b);
    auto C = d1::point(r.l, r.t - r.height() / 2.0);
    auto D = d1::point(r.r, C.y);

    c.adjustMarker(g, A, AMI::Horiz(), isTarget);
    c.adjustMarker(g, B, AMI::Horiz(), isTarget);
    c.adjustMarker(g, C, AMI::Vert(), isTarget);
    c.adjustMarker(g, D, AMI::Vert(), isTarget);
}

}
