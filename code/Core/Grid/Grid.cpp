/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Grid;

import std;


namespace Core
{

/*
  what ldiv() does
  ----------------

  ldiv_t d = ldiv(nom, denom);

  POST:

  d.quot * denom + d.rem = nom

  d.quot has the same sign as (nom / denom)

  if nom>0 and denom>0, then d.quot>0 and d.rem >= 0
  if nom<0 and denom>0, then d.quot<0 and d.rem <= 0
*/

namespace
{
constexpr long HALF_SPACING = 50L;
constexpr long SPACING = 2 * HALF_SPACING;

d1::int32 round(d1::int32 x)
{
    std::ldiv_t d = std::ldiv(x, SPACING);
    if (x >= 0)
    {
        D1_ASSERT(d.rem >= 0);
        if (d.rem > HALF_SPACING)
            return (d.quot + 1) * SPACING;
    }
    else
    {
        D1_ASSERT(d.rem <= 0);
        if (d.rem < -HALF_SPACING)
            return (d.quot - 1) * SPACING;
    }
    return d.quot * SPACING;
}

d1::int32 round_up(d1::int32 x)
{
    ldiv_t d = ldiv(x, SPACING);
    if (x >= 0)
        return (d.rem ? (d.quot + 1) * SPACING : d.quot * SPACING);

    return d.quot * SPACING;
}

d1::int32 round_down(d1::int32 x)
{
    ldiv_t d = ldiv(x, SPACING);
    if (x >= 0)
        return (d.quot * SPACING);

    return (d.rem ? (d.quot - 1) * SPACING : d.quot * SPACING);
}

struct DefaultGrid: public IGrid
{
    //-- Core::IGrid

    d1::Point toGrid(const d1::Point&) const override;
    d1::int32 enlarge(d1::int32 d) const override;
    d1::Vector enlarge(const d1::Vector& v) const override;
    d1::Size enlarge(const d1::Size& s) const override;
    d1::nRect enlarge(const d1::nRect&) const override;

};

using C = DefaultGrid;

}


d1::Point C::toGrid(const d1::Point& p) const
{
    return { round(p.x), round(p.y) };
}


d1::int32 C::enlarge(d1::int32 d) const
{
    return round_up(d);
}


d1::Vector C::enlarge(const d1::Vector& v) const
{
    return { round_up(v.dx), round_up(v.dy) };
}


d1::Size C::enlarge(const d1::Size& s) const
{
    return { round_up(s.w), round_up(s.h) };
}


d1::nRect C::enlarge(const d1::nRect& r) const
{
    return d1::nRect::A{
        .left = round_down(r.l),
        .top = round_up(r.t),
        .right = round_up(r.r),
        .bottom = round_down(r.b)
    };
}


const IGrid& IGrid::get()
{
    static DefaultGrid g;
    return g;
}

}
