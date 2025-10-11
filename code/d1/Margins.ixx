/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */
module;

#include "d1assert.h"

export module d1.Margins;

export import d1.types;

import d1.isEqual;
import d1.round;


namespace d1
{

//
// struct definitions
//

export template <class V>
struct MarginsBase
{
    using value_type = V;

    value_type
        l{}, // left
        t{}, // top
        r{}, // right
        b{}; // bottom

    value_type Left() const { return l; }
    value_type Top() const { return t; }
    value_type Right() const { return r; }
    value_type Bottom() const { return b; }
};


export struct Margins: public MarginsBase<int32>
{
    using B = MarginsBase<int32>;

    using value_type = B::value_type;

    Margins() {}

    constexpr Margins(
        value_type left,
        value_type top,
        value_type right,
        value_type bottom):
        B{
            .l = left,
            .t = top,
            .r = right,
            .b = bottom
        }
    {
    }

    bool operator==(const Margins& x) const
    {
        return (l == x.l) && (t == x.t) && (r == x.r) && (b == x.b);
    }
};


export struct fMargins: public MarginsBase<float64>
{
    using B = MarginsBase<float64>;

    using value_type = B::value_type;

    fMargins() {}

    fMargins(std::initializer_list<value_type> list)
    {
        D1_ASSERT((list.size() == 4))
        auto i = begin(list);
        auto e = end(list);
        if (i != e)
            B::l = *i++;
        if (i != e)
            B::t = *i++;
        if (i != e)
            B::r = *i++;
        if (i != e)
            B::b = *i;
    }

    fMargins(std::initializer_list<int32> list)
    {
        D1_ASSERT((list.size() == 4))
        auto i = begin(list);
        auto e = end(list);
        if (i != e)
            B::l = static_cast<value_type>(*i++);
        if (i != e)
            B::t = static_cast<value_type>(*i++);
        if (i != e)
            B::r = static_cast<value_type>(*i++);
        if (i != e)
            B::b = static_cast<value_type>(*i);
    }

    fMargins(const Margins& x):
        B{
            .l = static_cast<value_type>(x.l),
            .t = static_cast<value_type>(x.t),
            .r = static_cast<value_type>(x.r),
            .b = static_cast<value_type>(x.b)
        }
    {
    }

    bool operator==(const fMargins& rhs) const
    {
        return isEqual(l, rhs.l) &&
               isEqual(t, rhs.t) &&
               isEqual(r, rhs.r) &&
               isEqual(b, rhs.b);
    }
};

//
// non-member functions
//

export inline Margins round(const fMargins& r)
{
    return { round(r.l), round(r.t), round(r.r), round(r.b) };
}

}
