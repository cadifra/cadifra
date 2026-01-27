/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import d1.isEqual;

import GraphUtil.Shapes;


namespace Core
{

using std::strong_ordering;

namespace
{

constexpr d1::float64 MinSize = 2.0;
constexpr d1::float64 MinDist = 0.1;

void fuzzyCompare(d1::float64& res, const d1::float64& min, d1::float64 a, d1::float64 b)
{
    a = std::max(min, std::fabs(a));
    b = std::max(min, std::fabs(b));
    if (not d1::isEqual(a, b))
        res += (a - b) / (a + b);
}

}


class Weight::Impl
{
    d1::int32 selectionBias_ = 0;

protected:
    ~Impl()
    {
    }

public:
    virtual void
    operator+=(const Impl& rhs)
    {
        selectionBias_ += rhs.selectionBias_;
    }

    virtual void increaseSelectionBias() { ++selectionBias_; }

    d1::int32 getSelectionBias() const { return selectionBias_; }

    class Invisible;
    class Control;
    class Text;
    class Point;
    class Line;
    class Area;

    // double dispatch:
    virtual strong_ordering compare(const Impl&) const = 0;

    virtual strong_ordering compare(const Control&) const = 0;
    virtual strong_ordering compare(const Text&) const = 0;
    virtual strong_ordering compare(const Point&) const = 0;
    virtual strong_ordering compare(const Line&) const = 0;
    virtual strong_ordering compare(const Area&) const = 0;

    strong_ordering invert(strong_ordering o) const
    {
        if (o == strong_ordering::less)
            return strong_ordering::greater;
        else if (o == strong_ordering::greater)
            return strong_ordering::less;
        return o;
    }

    virtual void print(std::ostream& s) const;
};


class Weight::Impl::Invisible: public Impl
{
public:
    ~Invisible()
    {
    }

    static std::shared_ptr<Invisible> instance();
    void operator+=(const Impl& rhs) {}
    void increaseSelectionBias() {}

    strong_ordering compare(const Impl& i) const { return strong_ordering::less; }

    strong_ordering compare(const Control&) const { return strong_ordering::less; }
    strong_ordering compare(const Text&) const { return strong_ordering::less; }
    strong_ordering compare(const Point&) const { return strong_ordering::less; }
    strong_ordering compare(const Line&) const { return strong_ordering::less; }
    strong_ordering compare(const Area&) const { return strong_ordering::less; }
};

auto Weight::Impl::Invisible::instance()
    -> std::shared_ptr<Impl::Invisible>
{
    static auto singleton = std::make_shared<Impl::Invisible>();
    return singleton;
}


class Weight::Impl::Control: public Impl
{
    const d1::float64 distance2_;

public:
    Control(const d1::Point& weightAt, const d1::fPoint& pos):
        distance2_{ d1::squareDistance(weightAt, pos) } {}

    strong_ordering operator<=>(const Control&) const;

    strong_ordering compare(const Impl& i) const { return invert(i.compare(*this)); }

    strong_ordering compare(const Control& r) const { return *this <=> r; }
    strong_ordering compare(const Text&) const { return strong_ordering::greater; }
    strong_ordering compare(const Point&) const { return strong_ordering::greater; }
    strong_ordering compare(const Line&) const { return strong_ordering::greater; }
    strong_ordering compare(const Area&) const { return strong_ordering::greater; }
};


strong_ordering Weight::Impl::Control::operator<=>(const Control& c) const
{
    if (not d1::isEqual(distance2_, c.distance2_))
    {
        if (distance2_ < c.distance2_)
            return strong_ordering::greater;
        return strong_ordering::less;
    }

    return getSelectionBias() <=> c.getSelectionBias();
}


class Weight::Impl::Text: public Impl
{
public:
    strong_ordering operator<=>(const Text&) const;

    strong_ordering compare(const Impl& i) const { return invert(i.compare(*this)); }

    strong_ordering compare(const Control&) const { return strong_ordering::less; }
    strong_ordering compare(const Text& r) const { return *this <=> r; }
    strong_ordering compare(const Point&) const { return strong_ordering::greater; }
    strong_ordering compare(const Line&) const { return strong_ordering::greater; }
    strong_ordering compare(const Area&) const { return strong_ordering::greater; }
};

strong_ordering Weight::Impl::Text::operator<=>(const Text& t) const
{
    return getSelectionBias() <=> t.getSelectionBias();
}


class Weight::Impl::Point: public Impl
{
    const d1::Point weightAt_;
    const d1::int32 fuzziness_;
    const d1::fPoint pos_;

public:
    Point(const d1::Point& weightAt, d1::int32 fuzziness, const d1::fPoint& pos):
        weightAt_{ weightAt },
        fuzziness_{ fuzziness },
        pos_{ pos }
    {
    }

    strong_ordering operator<=>(const Point&) const;
    strong_ordering operator<=>(const Line&) const;
    strong_ordering operator<=>(const Area&) const;

    strong_ordering compare(const Impl& i) const { return invert(i.compare(*this)); }

    strong_ordering compare(const Control& r) const { return strong_ordering::less; }
    strong_ordering compare(const Text& r) const { return strong_ordering::less; }
    strong_ordering compare(const Point& r) const { return *this <=> r; }
    strong_ordering compare(const Line& r) const { return *this <=> r; }
    strong_ordering compare(const Area& r) const { return *this <=> r; }

private:
    d1::float64 distance2() const { return d1::squareDistance(weightAt_, pos_); }
};


class Weight::Impl::Line: public Impl
{
    const d1::Point weightAt_;
    const d1::int32 fuzziness_;
    const d1::fPoint A_;
    const d1::fPoint B_;

public:
    Line(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& a, const d1::fPoint& b):
        weightAt_{ weightAt },
        fuzziness_{ fuzziness },
        A_{ a },
        B_{ b }
    {
    }

    strong_ordering operator<=>(const Line&) const;
    strong_ordering operator<=>(const Area&) const;

    strong_ordering compare(const Impl& i) const { return invert(i.compare(*this)); }

    strong_ordering compare(const Control& r) const { return strong_ordering::less; }
    strong_ordering compare(const Text& r) const { return strong_ordering::less; }
    strong_ordering compare(const Point& r) const { return *this <=> r; }
    strong_ordering compare(const Line& r) const { return *this <=> r; }
    strong_ordering compare(const Area& r) const { return *this <=> r; }


    d1::float64 centerDistance2() const
    {
        return d1::squareDistance(weightAt_, center());
    }

    d1::float64 length2() const { return d1::squareDistance(A_, B_); }

private:
    d1::float64 distance2() const
    {
        return GraphUtil::Segment{ A_, B_ }.squareDistance(weightAt_);
    }

    d1::fPoint center() const
    {
        return { (A_.x + B_.x) / 2.0, (A_.y + B_.y) / 2.0 };
    }
};


class Weight::Impl::Area: public Impl
{
    const d1::Point weightAt_;
    const d1::int32 fuzziness_;
    const d1::nRect rect_;

public:
    Area(const d1::Point& weightAt, d1::int32 fuzziness, const d1::nRect& r):
        weightAt_{ weightAt },
        fuzziness_{ fuzziness },
        rect_{ r }
    {
    }

    strong_ordering operator<=>(const Area&) const;

    strong_ordering compare(const Impl& i) const { return invert(i.compare(*this)); }

    strong_ordering compare(const Control& r) const { return strong_ordering::less; }
    strong_ordering compare(const Text& r) const { return strong_ordering::less; }
    strong_ordering compare(const Point& r) const { return *this <=> r; }
    strong_ordering compare(const Line& r) const { return *this <=> r; }
    strong_ordering compare(const Area& r) const { return *this <=> r; }

    d1::float64 centerDistance2() const
    {
        return d1::squareDistance(weightAt_, rect_.center());
    }

    d1::int32 width() const { return rect_.width(); }
    d1::int32 height() const { return rect_.height(); }

private:
    d1::float64 distance2() const;
    bool hit() const;
    d1::int32 size() const { return rect_.width() * rect_.height(); }
};


strong_ordering Weight::Impl::Point::operator<=>(const Point& r) const
{
    if (pos_ == r.pos_)
        return getSelectionBias() <=> r.getSelectionBias();

    return std::strong_order(distance2(), r.distance2());
}


strong_ordering Weight::Impl::Point::operator<=>(const Line& r) const
{
    d1::float64 a = 0;

    fuzzyCompare(a, MinSize * fuzziness_, 0, std::sqrt(r.length2()));
    fuzzyCompare(a, MinDist * fuzziness_,
        std::sqrt(distance2()), std::sqrt(r.centerDistance2()));

    if (a < 0)
        return strong_ordering::greater;
    return strong_ordering::less;
}


strong_ordering Weight::Impl::Point::operator<=>(const Area& r) const
{
    d1::float64 a = 0;

    fuzzyCompare(a, MinSize * fuzziness_, 0, r.width());
    fuzzyCompare(a, MinSize * fuzziness_, 0, r.height());
    fuzzyCompare(a, MinDist * fuzziness_,
        std::sqrt(distance2()), std::sqrt(r.centerDistance2()));

    if (a < 0)
        return strong_ordering::greater;
    return strong_ordering::less;
}


strong_ordering Weight::Impl::Line::operator<=>(const Line& r) const
{
    const d1::float64 dist = distance2();
    const d1::float64 distR = r.distance2();

    if (not d1::isEqual(dist, distR))
    {
        if (dist < distR)
            return strong_ordering::greater;
        return strong_ordering::less;
    }

    const d1::float64 length = length2();
    const d1::float64 lengthR = r.length2();

    if (not d1::isEqual(length, lengthR))
    {
        if (length < lengthR)
            return strong_ordering::greater;
        return strong_ordering::less;
    }

    return getSelectionBias() <=> r.getSelectionBias();
}


strong_ordering Weight::Impl::Line::operator<=>(const Area& r) const
{
    d1::float64 a = 0;

    fuzzyCompare(a, MinSize * fuzziness_, A_.x - B_.x, r.width());
    fuzzyCompare(a, MinSize * fuzziness_, A_.y - B_.y, r.height());
    fuzzyCompare(a, MinDist * fuzziness_,
        std::sqrt(distance2()), std::sqrt(r.centerDistance2()));

    if (a < 0)
        return strong_ordering::greater;
    return strong_ordering::less;
}


strong_ordering Weight::Impl::Area::operator<=>(const Area& r) const
{
    const d1::int32 s = size();
    const d1::int32 sizeR = r.size();

    if (s != sizeR)
    {
        if (s > sizeR)
            return invert(r.compare(*this));

        D1_ASSERT(s < sizeR);

        if (r.rect_.encloses(rect_))
            return strong_ordering::greater;

        if (not hit() and r.hit())
            return strong_ordering::less;

        return strong_ordering::greater;
    }

    const d1::float64 dist = distance2();
    const d1::float64 distR = r.distance2();

    if (not d1::isEqual(dist, distR))
    {
        if (dist < distR)
            return strong_ordering::greater;
        return strong_ordering::less;
    }

    return getSelectionBias() <=> r.getSelectionBias();
}


bool Weight::Impl::Area::hit() const
{
    return copy(rect_).enlarge(fuzziness_).encloses(weightAt_);
}


d1::float64 Weight::Impl::Area::distance2() const
{
    if (rect_.encloses(weightAt_))
        return 0;

    d1::float64 h = 0;

    if (weightAt_.x < rect_.l)
        h = rect_.l - weightAt_.x;
    else if (weightAt_.x > rect_.r)
        h = weightAt_.x - rect_.r;

    d1::float64 v = 0;

    if (weightAt_.y < rect_.b)
        v = rect_.b - weightAt_.y;
    else if (weightAt_.y > rect_.t)
        v = weightAt_.y - rect_.t;

    return h * h + v * v;
}


Weight::Weight(const std::shared_ptr<Impl>& impl):
    impl_{ impl }
{
}


Weight::Weight():
    impl_{ Impl::Invisible::instance() }
{
}


Weight::~Weight() = default;


std::strong_ordering Weight::operator<=>(const Weight& rhs) const
{
    return impl_->compare(*rhs.impl_);
}


bool Weight::operator==(const Weight& rhs) const
{
    return not (*this < rhs) and not (rhs < *this);
}


void Weight::operator+=(const Weight& rhs)
{
    *impl_ += *rhs.impl_;
}


void Weight::increaseSelectionBias(const IView* v, const IElement& m)
{
    if (m.isSelected(v))
        impl_->increaseSelectionBias();
}


auto Weight::invisible() -> Weight
{
    return { Impl::Invisible::instance() };
}


auto Weight::text() -> Weight
{
    return { std::make_shared<Impl::Text>() };
}


auto Weight::control(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& point) -> Weight
{
    return { std::make_shared<Impl::Control>(weightAt, point) };
}


auto Weight::point(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& point) -> Weight
{
    return { std::make_shared<Impl::Point>(weightAt, fuzziness, point) };
}


auto Weight::line(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& a, const d1::fPoint& b) -> Weight
{
    return { std::make_shared<Impl::Line>(weightAt, fuzziness, a, b) };
}


auto Weight::area(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::nRect& r) -> Weight
{
    return { std::make_shared<Impl::Area>(weightAt, fuzziness, r) };
}


void Weight::print(std::ostream& s) const
{
    impl_->print(s);
}


void Weight::Impl::print(std::ostream& s) const
{
#ifdef _DEBUG
    s << "selectionBias_=" << selectionBias_ << " " << typeid(*this).name();
#endif
}

}
