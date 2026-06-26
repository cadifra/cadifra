/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.Weight;

import d1.Rect;
import d1.isEqual;

import GraphUtil.Segment;

import std;


namespace Core
{

using std::strong_ordering;

class Control;
class Text;
class Point;
class Line;
class Area;


class WeightImpl
{
    d1::int32 selectionBias_ = 0;

public:
    virtual void
    operator+=(const WeightImpl& rhs)
    {
        selectionBias_ += rhs.selectionBias_;
    }

    virtual void increaseSelectionBias() { ++selectionBias_; }

    d1::int32 getSelectionBias() const { return selectionBias_; }

    virtual void print(std::ostream& s) const;
};


class Invisible: public WeightImpl
{
public:
    void operator+=(const WeightImpl& rhs) {}
    void increaseSelectionBias() {}
};


class Control: public WeightImpl
{
    d1::float64 distance2_;

public:
    Control(const d1::Point& weightAt, const d1::fPoint& pos):
        distance2_{ d1::squareDistance(weightAt, pos) } {}

    strong_ordering operator<=>(const Control&) const;
};


class Text: public WeightImpl
{
public:
    strong_ordering operator<=>(const Text&) const;
};


class Point: public WeightImpl
{
    d1::Point weightAt_;
    d1::fPoint pos_;

public:
    Point(const d1::Point& weightAt, d1::int32 fuzziness, const d1::fPoint& pos):
        weightAt_{ weightAt },
        pos_{ pos }
    {
    }

    strong_ordering operator<=>(const Point&) const;

private:
    d1::float64 distance2() const { return d1::squareDistance(weightAt_, pos_); }
};


class Line: public WeightImpl
{
    d1::Point weightAt_;
    d1::int32 fuzziness_;
    d1::fPoint A_;
    d1::fPoint B_;

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

    d1::float64 length2() const { return d1::squareDistance(A_, B_); }

private:
    d1::float64 distance2() const
    {
        return GraphUtil::Segment{ A_, B_ }.squareDistance(weightAt_);
    }
};


class Area: public WeightImpl
{
    d1::Point weightAt_;
    d1::int32 fuzziness_;
    d1::nRect rect_;

public:
    Area(const d1::Point& weightAt, d1::int32 fuzziness, const d1::nRect& r):
        weightAt_{ weightAt },
        fuzziness_{ fuzziness },
        rect_{ r }
    {
    }

    strong_ordering operator<=>(const Area&) const;

    strong_ordering compare(const Area& r) const { return *this <=> r; }

    d1::int32 width() const { return rect_.width(); }
    d1::int32 height() const { return rect_.height(); }

private:
    d1::float64 distance2() const;
    bool hit() const;
    d1::int32 size() const { return rect_.width() * rect_.height(); }
};


export class Weight
//
//  A weight which is attached to model and view elements.
//  An element with a higher weight is preferred in function
//  IView::FindPointable.
//  "Higher" means in terms of its operator>().
//
{
    using Impl = std::variant< // the order matters, higher index -> higher priority
        Invisible, Area, Line, Point, Text, Control>;

    Impl impl_;

    Weight(const Impl& impl):
        impl_{ impl }
    {
    }

    auto getImpl() -> WeightImpl*;
    auto getImpl() const -> const WeightImpl*;

    template <class T>
    auto compare(const Weight&) const -> std::optional<std::strong_ordering>;

public:
    Weight();

    Weight(const Weight&) = default;
    Weight& operator=(const Weight&) = default;

    static auto invisible() -> Weight;
    static auto text() -> Weight;

    static auto control(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& point) -> Weight;

    static auto point(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& point) -> Weight;

    static auto line(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& a, const d1::fPoint& b) -> Weight;

    static auto area(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::nRect& r) -> Weight;

    void increaseSelectionBias();

    void operator+=(const Weight& rhs);

    bool operator==(const Weight& rhs) const;
    std::strong_ordering operator<=>(const Weight& rhs) const;

    void print(std::ostream& s) const;

    friend auto operator<<(std::ostream& s, const Weight& w) -> std::ostream&
    {
        w.print(s);
        return s;
    }
};

}

module :private;


namespace Core
{

Weight::Weight():
    impl_{ Invisible() }
{
}


auto Weight::getImpl() -> WeightImpl*
{
    const auto* cw = this;
    const auto* wi = cw->getImpl();

    return const_cast<WeightImpl*>(wi);
}


auto Weight::getImpl() const -> const WeightImpl*
{
    const WeightImpl* wi = nullptr;

    if (!wi)
        wi = std::get_if<Invisible>(&impl_);
    if (!wi)
        wi = std::get_if<Text>(&impl_);
    if (!wi)
        wi = std::get_if<Control>(&impl_);
    if (!wi)
        wi = std::get_if<Point>(&impl_);
    if (!wi)
        wi = std::get_if<Line>(&impl_);
    if (!wi)
        wi = std::get_if<Area>(&impl_);

    D1_ASSERT(wi);

    return wi;
}


void Weight::operator+=(const Weight& rhs)
{
    const auto* rwi = rhs.getImpl();
    auto* wi = getImpl();

    if (wi and rwi)
        *wi += *rwi;
}


void Weight::increaseSelectionBias()
{
    auto* wi = getImpl();

    if (wi)
        wi->increaseSelectionBias();
}


auto Weight::invisible() -> Weight
{
    return { Invisible() };
}


auto Weight::text() -> Weight
{
    return { Text() };
}


auto Weight::control(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& point) -> Weight
{
    return { Control(weightAt, point) };
}


auto Weight::point(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& point) -> Weight
{
    return { Point(weightAt, fuzziness, point) };
}


auto Weight::line(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& a, const d1::fPoint& b) -> Weight
{
    return { Line(weightAt, fuzziness, a, b) };
}


auto Weight::area(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::nRect& r) -> Weight
{
    return { Area(weightAt, fuzziness, r) };
}


bool Weight::operator==(const Weight& rhs) const
{
    return not(*this < rhs) and not(rhs < *this);
}


template <class T>
auto Weight::compare(const Weight& rhs) const
    -> std::optional<std::strong_ordering>
{
    if (auto* c = std::get_if<T>(&impl_))
        return *c <=> std::get<T>(rhs.impl_);

    return std::nullopt;
}


std::strong_ordering Weight::operator<=>(const Weight& rhs) const
{
    if (impl_.index() == 0)
        return strong_ordering::less; // this is invisible

    if (impl_.index() < rhs.impl_.index())
        return strong_ordering::less;

    if (impl_.index() > rhs.impl_.index())
        return strong_ordering::greater;

    D1_ASSERT(impl_.index() == rhs.impl_.index());

    if (auto res = compare<Control>(rhs))
        return res.value();

    if (auto res = compare<Text>(rhs))
        return res.value();

    if (auto res = compare<Point>(rhs))
        return res.value();

    if (auto res = compare<Line>(rhs))
        return res.value();

    if (auto res = compare<Area>(rhs))
        return res.value();

    return strong_ordering::less;
}


void Weight::print(std::ostream& s) const
{
    const auto* wi = getImpl();

    if (wi)
        wi->print(s);
}


void WeightImpl::print(std::ostream& s) const
{
#ifdef _DEBUG
    s << "selectionBias_=" << selectionBias_ << " " << typeid(*this).name();
#endif
}


strong_ordering invert(strong_ordering o)
{
    if (o == strong_ordering::less)
        return strong_ordering::greater;
    else if (o == strong_ordering::greater)
        return strong_ordering::less;
    return o;
}


strong_ordering Control::operator<=>(const Control& c) const
{
    if (not d1::isEqual(distance2_, c.distance2_))
    {
        if (distance2_ < c.distance2_)
            return strong_ordering::greater;
        return strong_ordering::less;
    }

    return getSelectionBias() <=> c.getSelectionBias();
}


strong_ordering Text::operator<=>(const Text& t) const
{
    return getSelectionBias() <=> t.getSelectionBias();
}


strong_ordering Point::operator<=>(const Point& r) const
{
    if (pos_ == r.pos_)
        return getSelectionBias() <=> r.getSelectionBias();

    return std::strong_order(distance2(), r.distance2());
}


strong_ordering Line::operator<=>(const Line& r) const
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


strong_ordering Area::operator<=>(const Area& r) const
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


bool Area::hit() const
{
    return copy(rect_).enlarge(fuzziness_).encloses(weightAt_);
}


d1::float64 Area::distance2() const
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

}
