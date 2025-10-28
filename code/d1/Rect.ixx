/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */
module;

#include "d1assert.h"

export module d1.Rect;

export import d1.types;
export import d1.Point;
export import d1.Size;

import d1.round;
import d1.isEqual;

import std;


namespace d1
{

//
// forward declarations
//

export class Rect;
export class fRect;
export class nRect;
export class fnRect;


//
// class definitions
//

template <typename P>
struct RectBase
{
    using value_type = P::value_type;

    value_type
        l{}, // left
        t{}, // top
        r{}, // right
        b{}; // bottom

    struct A
    {
        value_type left{}, top{}, right{}, bottom{};
    };

    value_type Left() const { return l; }
    value_type Top() const { return t; }
    value_type Right() const { return r; }
    value_type Bottom() const { return b; }

    P Center() const
    {
        return { (r + l) / 2, (t + b) / 2 };
    }

    template <class V>
    void Translate(const V& v)
    {
        l += v.dx;
        r += v.dx;
        t += v.dy;
        b += v.dy;
    }
};


export class Rect: public RectBase<Point>
{
    using B = RectBase;

public:
    using value_type = B::value_type;

    Rect() {}

    constexpr Rect(
        value_type left,
        value_type top,
        value_type right,
        value_type bottom):
        B{
            left,
            top,
            right,
            bottom
        }
    {
    }

    constexpr Rect(const Point& topleft, const Point& bottomright):
        B{
            topleft.x,
            topleft.y,
            bottomright.x,
            bottomright.y
        }
    {
    }

    constexpr Rect(const Point& topleft, int32 width, int32 height):
        B{
            topleft.x,
            topleft.y,
            topleft.x + width,
            topleft.y - height
        }
    {
    }

    constexpr Rect(const Point& topleft, const Size& s):
        B{
            topleft.x,
            topleft.y,
            topleft.x + s.w,
            topleft.y - s.h
        }
    {
    }

    bool operator==(const Rect& x) const
    {
        return (l == x.l) && (t == x.t) && (r == x.r) && (b == x.b);
    }

    auto& Translate(const Vector& v)
    {
        B::Translate(v);
        return *this;
    }

    friend auto Translate(const Rect& r, const Vector& v)
    {
        auto res = r;
        return res.Translate(v);
    }
};


export class fRect: public RectBase<fPoint>
{
    using B = RectBase;

public:
    using value_type = B::value_type;

    fRect() {}

    fRect(
        value_type left,
        value_type top,
        value_type right,
        value_type bottom):
        B{
            left,
            top,
            right,
            bottom
        }
    {
    }

    bool operator==(const fRect& rhs) const
    {
        return isEqual(l, rhs.l) &&
               isEqual(t, rhs.t) &&
               isEqual(r, rhs.r) &&
               isEqual(b, rhs.b);
    }
};


//
//  Normalized rectangles. Normalized means l <= r and t >= b
//

template <class P>
class nRectBase: public RectBase<P>
{
    using B = RectBase<P>;

public:
    using value_type = B::value_type;
    using B::l, B::t, B::r, B::b;

    void Normalize()
    {
        if (l > r)
            std::swap(l, r);
        if (b > t)
            std::swap(b, t);
    }

    bool IsNormalized() const { return (l <= r) && (b <= t); }

    value_type Width() const { return r - l; }
    value_type Height() const { return t - b; }

    P TopLeft() const { return { l, t }; }
    P TopRight() const { return { r, t }; }
    P BottomLeft() const { return { l, b }; }
    P BottomRight() const { return { r, b }; }

    // PRE: this must be normalized
    void Enlarge(value_type delta)
    {
        D1_ASSERT(IsNormalized());
        l -= delta;
        r += delta;
        t += delta;
        b -= delta;
    }

    // PRE: this must be normalized
    bool Encloses(const P& p) const
    {
        D1_ASSERT(IsNormalized());
        return ((l <= p.x) && (r >= p.x) && (t >= p.y) && (b <= p.y));
    }

    // PRE: this and rhs must be normalized
    bool Encloses(const nRectBase& rhs) const
    {
        D1_ASSERT(IsNormalized());
        D1_ASSERT(rhs.IsNormalized());
        return ((l <= rhs.l) && (r >= rhs.r) && (t >= rhs.t) && (b <= rhs.b));
    }

    nRectBase() {}

    explicit nRectBase(
        value_type left,
        value_type top,
        value_type right,
        value_type bottom):
        B{ left, top, right, bottom }
    {
        Normalize();
    }
};


export class nRect: public nRectBase<Point>
{
    using B = nRectBase;

public:
    using value_type = B::value_type;
    using B::l, B::t, B::r, B::b;

    nRect() {}

    explicit nRect(
        value_type left,
        value_type top,
        value_type right,
        value_type bottom):
        B{ left, top, right, bottom }
    {
    }

    nRect(const A& a):
        B{
            a.left,
            a.top,
            a.right,
            a.bottom
        }

    {
    }

    nRect(const Point& topleft, const Point& bottomright):
        B{
            topleft.x,
            topleft.y,
            bottomright.x,
            bottomright.y
        }
    {
    }

    nRect(const Point& topleft, int32 width, int32 height):
        B{
            topleft.x,
            topleft.y,
            topleft.x + width,
            topleft.y - height
        }
    {
    }

    nRect(const Point& topleft, const Size& s):
        B{
            topleft.x,
            topleft.y,
            topleft.x + s.w,
            topleft.y - s.h
        }
    {
    }

    explicit nRect(const Rect& rhs):
        B{ rhs.l, rhs.t, rhs.r, rhs.b }
    {
    }

    bool operator==(const nRect& x) const
    {
        return (l == x.l) && (t == x.t) && (r == x.r) && (b == x.b);
    }

    nRect& operator+=(this nRect&, const nRect&);

    Size Size() const
    {
        return { r - l, t - b };
    }

    auto& Translate(const Vector& v)
    {
        B::Translate(v);
        return *this;
    }

    auto& Enlarge(value_type d)
    {
        B::Enlarge(d);
        return *this;
    }

    bool Encloses(const d1::Point& p) const
    {
        return B::Encloses(p);
    }

    bool Encloses(const d1::nRect& rhs) const
    {
        return B::Encloses(rhs);
    }
};


export class fnRect: public nRectBase<fPoint>
{
    using B = nRectBase;

public:
    using value_type = B::value_type;

    fnRect() {}

    fnRect(
        value_type left,
        value_type top,
        value_type right,
        value_type bottom):
        B{
            left,
            top,
            right,
            bottom
        }
    {
    }

    fnRect(const fPoint& topleft, const fPoint& bottomright):
        B{
            topleft.x,
            topleft.y,
            bottomright.x,
            bottomright.y
        }
    {
    }

    fnRect(const fPoint& topleft, float64 width, float64 height):
        B{
            topleft.x,
            topleft.y,
            topleft.x + width,
            topleft.y - height
        }
    {
    }

    fnRect(const fPoint& topleft, const Size& s):
        B{
            topleft.x,
            topleft.y,
            topleft.x + s.w,
            topleft.y - s.h
        }
    {
    }

    fnRect(const fPoint& topleft, const fSize& s):
        B{
            topleft.x,
            topleft.y,
            topleft.x + s.w,
            topleft.y - s.h
        }
    {
    }

    fnRect(const nRect& x):
        B{
            static_cast<value_type>(x.l),
            static_cast<value_type>(x.t),
            static_cast<value_type>(x.r),
            static_cast<value_type>(x.b)
        }
    {
    }

    fSize Size() const
    {
        return { r - l, t - b };
    }

    fnRect& operator+=(const fnRect& rect);

    auto& Translate(const fVector& v)
    {
        B::Translate(v);
        return *this;
    }

    auto& Enlarge(value_type d)
    {
        B::Enlarge(d);
        return *this;
    }

    bool Encloses(const fnRect& rhs) const
    {
        return B::Encloses(rhs);
    }

    bool Encloses(const fPoint& p) const
    {
        return B::Encloses(p);
    }

    bool operator==(const fnRect& rhs) const
    {
        return isEqual(l, rhs.l) &&
               isEqual(t, rhs.t) &&
               isEqual(r, rhs.r) &&
               isEqual(b, rhs.b);
    }

    friend auto round(const fnRect& r)
    {
        return nRect{ round(r.l), round(r.t), round(r.r), round(r.b) };
    }
};

//
// member functions
//

inline nRect& nRect::operator+=(this nRect& self, const nRect& y)
{
    // sets self to bounding box of self and y

    D1_ASSERT(y != nRect{});

    if (self == nRect{})
    {
        self = y;
        return self;
    }

    self = nRect::A{
        .left = std::min(self.l, y.l),
        .top = std::max(self.t, y.t),
        .right = std::max(self.r, y.r),
        .bottom = std::min(self.b, y.b)
    };

    return self;
}


inline fnRect& fnRect::operator+=(const fnRect& rect)
{
    D1_ASSERT(*this != fnRect{});
    D1_ASSERT(rect != fnRect{});
    D1_ASSERT(IsNormalized());
    D1_ASSERT(rect.IsNormalized());

    l = std::min(rect.l, l);
    t = std::max(rect.t, t);
    r = std::max(rect.r, r);
    b = std::min(rect.b, b);

    return *this;
}


//
// non-member functions
//

export {

auto nrect(const Point& p, const Size& s)
{
    return nRect{ p, s };
}


auto nrect(const fPoint& p, const fSize& s)
{
    return fnRect{ p, s };
}


auto nrect(const fPoint& p, const Size& s)
{
    return fnRect{ p, s };
}


auto nrect(const Point& p, int32 width, int32 height)
{
    return nRect{ p, width, height };
}


auto nrect(const fPoint& p, float64 width, float64 height)
{
    return fnRect{ p, width, height };
}


auto nrect(const Point& p1, const Point& p2)
{
    return nRect{ p1, p2 };
}


auto nrect(const fPoint& p1, const fPoint& p2)
{
    return fnRect{ p1, p2 };
}


auto copy(const nRect& r)
{
    return r;
}


auto copy(const fnRect& r)
{
    return r;
}


auto pos(const nRect& r)
{
    return r.TopLeft();
}


auto pos(const fnRect& r)
{
    return r.TopLeft();
}
}

}
