/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Base;

import Canvas.Brush;
import Canvas.ICustomDrawer;

import d1.Margins;
import d1.Rect;

import std;


namespace Canvas
{

export class AdjustMarkerInfo
{
public:
    bool left = true;
    bool right = true;
    bool up = true;
    bool down = true;

    using AMI = AdjustMarkerInfo;

    static AMI LeftUp()
    {
        return { 1, 0, 1, 0 };
    }
    static AMI LeftDown()
    {
        return { 1, 0, 0, 1 };
    }
    static AMI RightUp()
    {
        return { 0, 1, 1, 0 };
    }
    static AMI RightDown()
    {
        return { 0, 1, 0, 1 };
    }
    static AMI Horiz()
    {
        return { 1, 1, 0, 0 };
    }
    static AMI Vert()
    {
        return { 0, 0, 1, 1 };
    }
    static AMI Left()
    {
        return { 1, 0, 0, 0 };
    }
    static AMI Right()
    {
        return { 0, 1, 0, 0 };
    }
    static AMI Up()
    {
        return { 0, 0, 1, 0 };
    }
    static AMI Down()
    {
        return { 0, 0, 0, 1 };
    }
};


export class IOrder
{
public:
    using Ref = std::shared_ptr<IOrder>;

    virtual ~IOrder() = default;

    static Ref createMax();
    static Ref createMin();

    bool operator<(const IOrder& o) const;

private:
    virtual bool isLessThan(const IOrder&) const = 0;

    virtual bool isMax() const { return false; }
    virtual bool isMin() const { return false; }
};


export class Order // value-semantics
{
    IOrder::Ref base_;
    d1::int32 level_;

public:
    Order(
        IOrder::Ref base = IOrder::createMax(),
        d1::int32 level = 0):

        base_{ base },
        level_{ level }
    {
    }

    bool operator<(const Order& o) const;

    Order(const Order&) = default;
    Order& operator=(const Order&) = default;
};


export namespace PageDefaults
{

constexpr d1::int32 Size = 1; // DMPAPER_LETTER;
constexpr d1::int32 Width = 21590;
constexpr d1::int32 Height = 27940;
constexpr bool Landscape = false;

constexpr d1::int32 Rows = 1;
constexpr d1::int32 Columns = 1;

constexpr d1::Margins
    Margins{ 1016, 1016, 1016, 1016 },
    CutGlueMargins{ 1016, 1016, 1016, 1016 };

constexpr d1::float64 Scale = 1.0;

}


export class PageInfo
{
public:
    d1::fSize Size = { PageDefaults::Width, PageDefaults::Height };
    bool Landscape = PageDefaults::Landscape;
    d1::float64 Scale = PageDefaults::Scale;
    d1::int32 Rows = PageDefaults::Rows;
    d1::int32 Cols = PageDefaults::Columns;
    d1::fMargins Margins = PageDefaults::Margins;
    d1::fMargins CutGlueMargins = PageDefaults::CutGlueMargins;
};


export class PictureDescription
{
    int resID_ = 0;

public:
    PictureDescription(int ResID):
        resID_{ ResID }
    {
    }
    PictureDescription() {}

    int getID() const { return resID_; }

    // uses compiler generated copy and assignment operator
};


export class IElementImp
{
public:
    virtual void move(const d1::fVector&) = 0;

    virtual ~IElementImp() = default;
};


class GroupImp: public IElementImp
{
public:
    GroupImp() {}

    GroupImp(const GroupImp&) = delete;
    GroupImp& operator=(const GroupImp&) = delete;

    void add(const std::shared_ptr<IElementImp>& e)
    {
        elements_.push_back(e);
    }

    //-- IElementImp

    void move(const d1::fVector& v) override
    {
        for (auto& e : elements_)
            e->move(v);
    }

    //--

private:
    std::vector<std::shared_ptr<IElementImp>> elements_;
};


export class Group
{
public:
    Group() = default;

    Group(const std::shared_ptr<GroupImp>& e):
        imp_{ e } {}

    void clear()
    {
        imp_ = {};
    }

    void move(const d1::fVector& v)
    {
        if (imp_)
            imp_->move(v);
    }

    operator bool() const { return imp_.get() != 0; }

    void add(const std::shared_ptr<IElementImp>& e)
    {
        if (not imp_)
            imp_ = std::make_shared<GroupImp>();
        imp_->add(e);
    }

private:
    std::shared_ptr<GroupImp> imp_;
};


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
