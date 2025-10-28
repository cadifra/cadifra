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

namespace
{

constexpr d1::float64 MinSize = 2.0;
constexpr d1::float64 MinDist = 0.1;

void fuzzyCompare(d1::float64& res, const d1::float64& min, d1::float64 a, d1::float64 b)
{
    a = std::max(min, std::fabs(a));
    b = std::max(min, std::fabs(b));
    if (!d1::isEqual(a, b))
        res += (a - b) / (a + b);
}

}


class Weight::Impl
{
    d1::int32 itsSelectionBias = 0;

protected:
    ~Impl()
    {
    }

public:
    virtual void
    operator+=(const Impl& rhs)
    {
        itsSelectionBias += rhs.itsSelectionBias;
    }

    virtual void IncreaseSelectionBias() { ++itsSelectionBias; }

    d1::int32 GetSelectionBias() const { return itsSelectionBias; }

    class Invisible;
    class Control;
    class Text;
    class Point;
    class Line;
    class Area;

    // double dispatch:
    virtual bool operator<(const Impl&) const = 0;

    virtual bool operator>(const Invisible&) const = 0;
    virtual bool operator>(const Control&) const = 0;
    virtual bool operator>(const Text&) const = 0;
    virtual bool operator>(const Point&) const = 0;
    virtual bool operator>(const Line&) const = 0;
    virtual bool operator>(const Area&) const = 0;

    virtual void Print(std::ostream& s) const;
};


class Weight::Impl::Invisible: public Impl
{
public:
    ~Invisible()
    {
    }

    static std::shared_ptr<Invisible> Instance();
    void operator+=(const Impl& rhs) {}
    void IncreaseSelectionBias() {}

    bool operator<(const Impl& i) const { return i > *this; }

    bool operator>(const Invisible&) const { return false; }
    bool operator>(const Control&) const { return false; }
    bool operator>(const Text&) const { return false; }
    bool operator>(const Point&) const { return false; }
    bool operator>(const Line&) const { return false; }
    bool operator>(const Area&) const { return false; }
};

auto Weight::Impl::Invisible::Instance()
    -> std::shared_ptr<Impl::Invisible>
{
    static auto singleton = std::make_shared<Impl::Invisible>();
    return singleton;
}


class Weight::Impl::Control: public Impl
{
    const d1::float64 itsDistance2;

public:
    Control(const d1::Point& weightAt, const d1::fPoint& pos):
        itsDistance2{ d1::SquareDistance(weightAt, pos) } {}

    bool operator<(const Impl& i) const { return i > *this; }

    bool operator>(const Invisible& r) const { return !(r > *this); }
    bool operator>(const Control&) const;
    bool operator>(const Text&) const { return true; }
    bool operator>(const Point&) const { return true; }
    bool operator>(const Line&) const { return true; }
    bool operator>(const Area&) const { return true; }
};

bool Weight::Impl::Control::operator>(const Control& c) const
{
    if (!d1::isEqual(itsDistance2, c.itsDistance2))
        return itsDistance2 < c.itsDistance2;

    return GetSelectionBias() > c.GetSelectionBias();
}


class Weight::Impl::Text: public Impl
{
public:
    bool operator<(const Impl& i) const { return i > *this; }

    bool operator>(const Invisible& r) const { return !(r > *this); }
    bool operator>(const Control& r) const { return !(r > *this); }
    bool operator>(const Text&) const;
    bool operator>(const Point&) const { return true; }
    bool operator>(const Line&) const { return true; }
    bool operator>(const Area&) const { return true; }
};

bool Weight::Impl::Text::operator>(const Text& t) const
{
    return GetSelectionBias() > t.GetSelectionBias();
}


class Weight::Impl::Point: public Impl
{
    const d1::Point itsWeightAt;
    const d1::int32 itsFuzziness;
    const d1::fPoint itsPos;

public:
    Point(const d1::Point& weightAt, d1::int32 fuzziness, const d1::fPoint& pos):
        itsWeightAt{ weightAt },
        itsFuzziness{ fuzziness },
        itsPos{ pos }
    {
    }

    bool operator<(const Impl& i) const { return i > *this; }

    bool operator>(const Invisible& r) const { return !(r > *this); }
    bool operator>(const Control& r) const { return !(r > *this); }
    bool operator>(const Text& r) const { return !(r > *this); }
    bool operator>(const Point&) const;
    bool operator>(const Line&) const;
    bool operator>(const Area&) const;

private:
    d1::float64 Distance2() const { return d1::SquareDistance(itsWeightAt, itsPos); }
};


class Weight::Impl::Line: public Impl
{
    const d1::Point itsWeightAt;
    const d1::int32 itsFuzziness;
    const d1::fPoint itsA;
    const d1::fPoint itsB;

public:
    Line(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& a, const d1::fPoint& b):
        itsWeightAt{ weightAt },
        itsFuzziness{ fuzziness },
        itsA{ a },
        itsB{ b }
    {
    }

    bool operator<(const Impl& i) const { return i > *this; }

    bool operator>(const Invisible& r) const { return !(r > *this); }
    bool operator>(const Control& r) const { return !(r > *this); }
    bool operator>(const Text& r) const { return !(r > *this); }
    bool operator>(const Point& r) const { return !(r > *this); }
    bool operator>(const Line&) const;
    bool operator>(const Area&) const;

    d1::float64 CenterDistance2() const
    {
        return d1::SquareDistance(itsWeightAt, Center());
    }

    d1::float64 Length2() const { return d1::SquareDistance(itsA, itsB); }

private:
    d1::float64 Distance2() const
    {
        return GraphUtil::Segment{ itsA, itsB }.SquareDistance(itsWeightAt);
    }

    d1::fPoint Center() const
    {
        return { (itsA.x + itsB.x) / 2.0, (itsA.y + itsB.y) / 2.0 };
    }
};


class Weight::Impl::Area: public Impl
{
    const d1::Point itsWeightAt;
    const d1::int32 itsFuzziness;
    const d1::nRect itsRect;

public:
    Area(const d1::Point& weightAt, d1::int32 fuzziness, const d1::nRect& r):
        itsWeightAt{ weightAt },
        itsFuzziness{ fuzziness },
        itsRect{ r }
    {
    }

    bool operator<(const Impl& i) const { return i > *this; }

    bool operator>(const Invisible& r) const { return !(r > *this); }
    bool operator>(const Control& r) const { return !(r > *this); }
    bool operator>(const Text& r) const { return !(r > *this); }
    bool operator>(const Point& r) const { return !(r > *this); }
    bool operator>(const Line& r) const { return !(r > *this); }
    bool operator>(const Area&) const;

    d1::float64 CenterDistance2() const
    {
        return d1::SquareDistance(itsWeightAt, itsRect.Center());
    }

    d1::int32 Width() const { return itsRect.Width(); }
    d1::int32 Height() const { return itsRect.Height(); }

private:
    d1::float64 Distance2() const;
    bool Hit() const;
    d1::int32 Size() const { return itsRect.Width() * itsRect.Height(); }
};


bool Weight::Impl::Point::operator>(const Point& r) const
{
    if (itsPos == r.itsPos)
        return GetSelectionBias() > r.GetSelectionBias();

    return Distance2() < r.Distance2();
}


bool Weight::Impl::Point::operator>(const Line& r) const
{
    d1::float64 a = 0;

    fuzzyCompare(a, MinSize * itsFuzziness, 0, std::sqrt(r.Length2()));
    fuzzyCompare(a, MinDist * itsFuzziness,
        std::sqrt(Distance2()), std::sqrt(r.CenterDistance2()));

    return a < 0;
}


bool Weight::Impl::Point::operator>(const Area& r) const
{
    d1::float64 a = 0;

    fuzzyCompare(a, MinSize * itsFuzziness, 0, r.Width());
    fuzzyCompare(a, MinSize * itsFuzziness, 0, r.Height());
    fuzzyCompare(a, MinDist * itsFuzziness,
        std::sqrt(Distance2()), std::sqrt(r.CenterDistance2()));

    return a < 0;
}


bool Weight::Impl::Line::operator>(const Line& r) const
{
    const d1::float64 dist = Distance2();
    const d1::float64 distR = r.Distance2();

    if (!d1::isEqual(dist, distR))
        return dist < distR;

    const d1::float64 length = Length2();
    const d1::float64 lengthR = r.Length2();

    if (!d1::isEqual(length, lengthR))
        return length < lengthR;

    return GetSelectionBias() > r.GetSelectionBias();
}


bool Weight::Impl::Line::operator>(const Area& r) const
{
    d1::float64 a = 0;

    fuzzyCompare(a, MinSize * itsFuzziness, itsA.x - itsB.x, r.Width());
    fuzzyCompare(a, MinSize * itsFuzziness, itsA.y - itsB.y, r.Height());
    fuzzyCompare(a, MinDist * itsFuzziness,
        std::sqrt(Distance2()), std::sqrt(r.CenterDistance2()));

    return a < 0;
}


bool Weight::Impl::Area::operator>(const Area& r) const
{
    const d1::int32 size = Size();
    const d1::int32 sizeR = r.Size();

    if (size != sizeR)
    {
        if (size > sizeR)
            return !(r > *this);

        D1_ASSERT(size < sizeR);

        if (r.itsRect.Encloses(itsRect))
            return true;

        if (!Hit() && r.Hit())
            return false;

        return true;
    }

    const d1::float64 dist = Distance2();
    const d1::float64 distR = r.Distance2();

    if (!d1::isEqual(dist, distR))
        return dist < distR;

    return GetSelectionBias() > r.GetSelectionBias();
}


bool Weight::Impl::Area::Hit() const
{
    return copy(itsRect).Enlarge(itsFuzziness).Encloses(itsWeightAt);
}


d1::float64 Weight::Impl::Area::Distance2() const
{
    if (itsRect.Encloses(itsWeightAt))
        return 0;

    d1::float64 h = 0;

    if (itsWeightAt.x < itsRect.l)
        h = itsRect.l - itsWeightAt.x;
    else if (itsWeightAt.x > itsRect.r)
        h = itsWeightAt.x - itsRect.r;

    d1::float64 v = 0;

    if (itsWeightAt.y < itsRect.b)
        v = itsRect.b - itsWeightAt.y;
    else if (itsWeightAt.y > itsRect.t)
        v = itsWeightAt.y - itsRect.t;

    return h * h + v * v;
}


Weight::Weight(const std::shared_ptr<Impl>& impl):
    itsImpl{ impl }
{
}


Weight::Weight():
    itsImpl{ Impl::Invisible::Instance() }
{
}


Weight::~Weight() = default;


bool Weight::operator<(const Weight& rhs) const
{
    return *itsImpl < *rhs.itsImpl;
}


bool Weight::operator==(const Weight& rhs) const
{
    return !(*this < rhs) && !(rhs < *this);
}


void Weight::operator+=(const Weight& rhs)
{
    *itsImpl += *rhs.itsImpl;
}


void Weight::IncreaseSelectionBias(const IView* v, const IElement& m)
{
    if (m.IsSelected(v))
        itsImpl->IncreaseSelectionBias();
}


auto Weight::Invisible() -> Weight
{
    return { Impl::Invisible::Instance() };
}


auto Weight::Text() -> Weight
{
    return { std::make_shared<Impl::Text>() };
}


auto Weight::Control(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& point) -> Weight
{
    return { std::make_shared<Impl::Control>(weightAt, point) };
}


auto Weight::Point(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& point) -> Weight
{
    return { std::make_shared<Impl::Point>(weightAt, fuzziness, point) };
}


auto Weight::Line(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::fPoint& a, const d1::fPoint& b) -> Weight
{
    return { std::make_shared<Impl::Line>(weightAt, fuzziness, a, b) };
}


auto Weight::Area(const d1::Point& weightAt, d1::int32 fuzziness,
    const d1::nRect& r) -> Weight
{
    return { std::make_shared<Impl::Area>(weightAt, fuzziness, r) };
}


void Weight::Print(std::ostream& s) const
{
    itsImpl->Print(s);
}


void Weight::Impl::Print(std::ostream& s) const
{
#ifdef _DEBUG
    s << "itsSelectionBias=" << itsSelectionBias << " " << typeid(*this).name();
#endif
}

}
